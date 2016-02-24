#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>

#include <zr_logger.h>
#include <zr_file.h>
#include <zr_str.h>
#include <zr_netutils.h>

/////////////////////////////////////////////////////////////////////////
//app_update_info
/////////////////////////////////////////////////////////////////////////

typedef struct app_update_info {
	bool is_enabled; // 是否开启更新功能模块
	bool is_auto; //是否自动更新
	bool mode; //更新模式 0 - 下载，绝对不提示；1-下载，不提示。但不匹配时提示。2-下载，绝对提示
	bool is_silent; //静默下载
	char url[MAX_URL]; //Update检查文件URL。
	char url_manual[MAX_URL]; //http://yourserver.net/yourpage 更新说明的页面，用于直接显示到界面上
	char url_detail[MAX_URL]; // http://yourserver.net/yourpage 更新说明的页面，用于链接“更多...”
	char url_override[MAX_URL]; //
	int interval; //更新检查间隔（秒），缺省为1天
	int interval_nt_download; //提示用户下载新版本的间隔时间（秒）。说明：用户可能在上次提示的时候没有更新，那么你就间隔这么长时间再提醒一次
	int interval_nt_restart; //更新后提示用户重启应用的间隔时间。
	int interval_timer; //对所有的更新Timer的检查间隔周期（毫秒），缺省5秒
	bool is_show_installed_ui; //	是否显示安装界面
	int incompatible_mode; //// 注：不明白

} app_update_info_t;

bool __GetPrivateProfileBool(char * sec_name, char * att_name, bool def_val,
		char * path) {
	char buf[128];
	char * def_val_s = def_val ? "true" : "false";
	GetPrivateProfileString(sec_name, att_name, def_val_s, buf, sizeof buf, path);
	return strcmp(buf, "true") == 0;
}

void app_update_info_inifile_read(char *path, app_update_info_t * p_auinfo) {
	if (ZR_FILE_SUCC != zr_fexist(path)) {
		zr_log_error_ec(0, "File isn't exist. path=%s", path);
		//		return;
	}

	char * sec_name = "updates";
	p_auinfo->is_enabled = __GetPrivateProfileBool(sec_name, "enabled", true,
			path);
	p_auinfo->is_auto = __GetPrivateProfileBool(sec_name, "auto", true, path);
	p_auinfo->mode = GetPrivateProfileInt(sec_name, "mode", 1, path);
	p_auinfo->is_silent = __GetPrivateProfileBool(sec_name, "scient", true,
			path);
	GetPrivateProfileString(sec_name, "url", "http://127.0.0.1/update", p_auinfo->url, sizeof (p_auinfo->url), path);
	GetPrivateProfileString(sec_name, "url_manual", "", p_auinfo->url_manual, sizeof (p_auinfo->url_manual), path);
	GetPrivateProfileString(sec_name, "url_detail", "", p_auinfo->url_detail, sizeof (p_auinfo->url_detail), path);
	GetPrivateProfileString(sec_name, "url_override", "", p_auinfo->url_override, sizeof (p_auinfo->url_override), path);
	p_auinfo->interval = GetPrivateProfileInt(sec_name, "interval", 86400, path);
	p_auinfo->interval_nt_download = GetPrivateProfileInt(sec_name, "interval_nt_download", 86400, path);
	p_auinfo->interval_nt_restart = GetPrivateProfileInt(sec_name, "interval_nt_restart", 1800, path);
	p_auinfo->interval_timer = GetPrivateProfileInt(sec_name, "interval_timer", 1800, path);
	p_auinfo->is_show_installed_ui = __GetPrivateProfileBool(sec_name,
			"show_installed_ui", true, path);
	p_auinfo->incompatible_mode = GetPrivateProfileInt(sec_name, "incompatible_mode", 0, path);
}

void app_update_info_print(app_update_info_t auinfo) {
	printf("is_enabled=%d\r\n", auinfo.is_enabled);
	printf("is_auto=%d\r\n", auinfo.is_auto);
	printf("mode=%d\r\n", auinfo.mode);
	printf("is_silent=%d\r\n", auinfo.is_silent);
	printf("url=%s\r\n", auinfo.url);
	printf("url_manual=%s\r\n", auinfo.url_manual);
	printf("url_detail=%s\r\n", auinfo.url_detail);
	printf("url_override=%s\r\n", auinfo.url_override);
	printf("interval=%d\r\n", auinfo.interval);
	printf("interval_nt_download=%d\r\n", auinfo.interval_nt_download);
	printf("interval_nt_restart=%d\r\n", auinfo.interval_nt_restart);
	printf("interval_timer=%d\r\n", auinfo.interval_timer);
	printf("is_show_installed_ui=%d\r\n", auinfo.is_show_installed_ui);
	printf("incompatible_mode=%d\r\n", auinfo.incompatible_mode);
}

/////////////////////////////////////////////////////////////////////////
//GF_HttpDownloadListener
/////////////////////////////////////////////////////////////////////////

class GF_HttpDownloadListener : public HttpDownloadListener {
public:
	GF_HttpDownloadListener(char * progresslogPath, int stepCount,
			bool isConsole);
	~GF_HttpDownloadListener();

	void onConnect(char * host, int port);
	void onConnected(char * host, int port);
	void onGetContentLength(int length);
	void onProgress(time_t spentSeconds, long allRecvSize);
	void onFinished();
private:
	char progresslogPath[MAX_PATH];
	int stepCount;
	bool isConsole;
	long contentLength;
	int progressCounter;
	int progressesPerStep;
	time_t spentSeconds;
	long allRecvSize;
	bool isFinished;

	void saveProgresslog();
};
GF_HttpDownloadListener::GF_HttpDownloadListener(char * _progresslogPath,
		int _stepCount = 50, bool _isConsole = false) {
	isConsole = _isConsole;
	ZeroMemory(progresslogPath, MAX_PATH);
	strcpy(progresslogPath, _progresslogPath);
	stepCount = _stepCount;
	progressCounter = 0;
	progressesPerStep = 0;
	spentSeconds = 0;
	allRecvSize = 0;
	isFinished = false;
}
GF_HttpDownloadListener::~GF_HttpDownloadListener() {
}

void GF_HttpDownloadListener::saveProgresslog() {
	DWORD NumberOfBytesWritten;

	HANDLE progressLogFh;
	progressLogFh
			= CreateFile(progresslogPath, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);

	char buf[1024];
	ZeroMemory(buf, 1024);

	sprintf(buf, "[Progress]\r\n");
	WriteFile(progressLogFh, buf, strlen(buf), &NumberOfBytesWritten, NULL);

	sprintf(buf, "len=%d\r\n", contentLength);
	WriteFile(progressLogFh, buf, strlen(buf), &NumberOfBytesWritten, NULL);

	sprintf(buf, "curLen=%d\r\n", allRecvSize);
	WriteFile(progressLogFh, buf, strlen(buf), &NumberOfBytesWritten, NULL);

	sprintf(buf, "finished=%s\r\n", isFinished ? "true" : "false");
	WriteFile(progressLogFh, buf, strlen(buf), &NumberOfBytesWritten, NULL);

	CloseHandle(progressLogFh);
}

void GF_HttpDownloadListener::onConnect(char * host, int port) {
	if (isConsole)
		printf("Connect to %s:%d. ", host, port);
}
void GF_HttpDownloadListener::onConnected(char * host, int port) {
	if (isConsole)
		printf("OK\r\n");
}
void GF_HttpDownloadListener::onGetContentLength(int length) {
	if (isConsole)
		printf("Content-Length = %d Bytes (%.2f MB)\r\n", length, (float)length
				/ 1024 / 1024);
	contentLength = length;
	progressesPerStep = length / 4096 / stepCount;
}

void GF_HttpDownloadListener::onProgress(time_t _spentSeconds, long _allRecvSize) {
	spentSeconds = _spentSeconds;
	allRecvSize = _allRecvSize;

	progressCounter ++;
	if (progressCounter >= progressesPerStep) {
		progressCounter = 0;
		saveProgresslog();
	}

	if (isConsole) {
		printf("\r");
		if (0 == spentSeconds)
			return;
		float speed = allRecvSize / spentSeconds;
		time_t tm_all = contentLength / speed;
		time_t tm_less = tm_all - spentSeconds;
		int all_recv_size_k = allRecvSize / 1024;
		printf("%.2f%% %d/%d KB (%.2f KBps) ", (float)allRecvSize * 100
				/contentLength, all_recv_size_k, contentLength / 1024, speed
				/ 1024);
	}
}

void GF_HttpDownloadListener::onFinished() {
	isFinished = true;
	saveProgresslog();
}

void start(char * url, char * localPath, char * progressLogPath) {
	WSAData wsaData;
	u_short wVersionRequested= MAKEWORD( 2, 1);
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		zr_log_error_ec(err, "Socket error");
		return;
	}

	GF_HttpDownloadListener listener(progressLogPath, 50);
	zr_http_download_url(url, localPath, &listener);

	WSACleanup();
}

void strcatw(char * buf, WCHAR wc) {
	int len = strlen(buf);
	char * c = (char *)&wc;
	if (*c != 0) {
		buf[len++] = *c;
		buf[len] = '\0';
		c ++;
	}
	if (*c != 0) {
		buf[len++] = *c;
		buf[len] = '\0';
		c ++;
	}
}

void strcatwchars(char * buf, LPWSTR ws) {
	int len = wcslen(ws);
	for (int i=0; i<len; i++)
		strcatw(buf, ws[i]);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
		int nCmdShow) {
	char cmd[MAX_URL] = { 0 };
	strcpy(cmd, GetCommandLineA());
	char * url_pos = strstr(cmd, "/url:");
	if (NULL == url_pos)
		return 0;
	char * local_pos = strstr(cmd, "/local:");
	if (NULL == local_pos)
		return 0;
	char * log_pos = strstr(cmd, "/log:");
	if (NULL == log_pos)
		return 0;

	*url_pos = '\0';
	*local_pos = '\0';
	*log_pos = '\0';

	char url[MAX_URL] = { 0 };
	char localPath[MAX_URL]= { 0 };
	char progressLogPath[MAX_URL]= { 0 };

	strcpy(url, url_pos + 5);
	strcpy(localPath, local_pos + 7);
	strcpy(progressLogPath, log_pos + 5);

	zr_strtrim(url);
	zr_strtrim(localPath);
	zr_strtrim(progressLogPath);

	//	MessageBoxA(NULL, url, url, MB_OK);
	//	MessageBoxA(NULL, localPath, localPath, MB_OK);
	//	MessageBoxA(NULL, progressLogPath, progressLogPath, MB_OK);

	start(url, localPath, progressLogPath);
	return 0;
}
/////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////
int main(int argc, char ** argv) {
	if (argc < 4) {
		printf("Usage:\r\ngf_updater.exe url localpath progressLogPath\r\n");
		return 0;
	}

	char * url = argv[1];
	char * localPath = argv[2];
	char * progressLogPath = argv[3];

	start(url, localPath, progressLogPath);
	return 0;
}
