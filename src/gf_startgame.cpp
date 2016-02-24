#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <detours.h>
#include <zr_sock.h>
#include <zr_file.h>
#include <zr_mem.h>

#include "gf_head.h"
#include "gf_props.h"
#include "gf_udpspd.h"
#include "ac_all.h"
#include "gf_hard.h"

#define GAME_ID_NONE	0
#define GAME_ID_WAR3	1
#define GAME_ID_CS15	2
#define GAME_ID_CS16	3
#define GAME_ID_STARCRAFT	4

int G_curGame= GAME_ID_NONE;

char game_dir[MAX_PATH];

HANDLE war3_porcess_handle = 0;
//GameHelper Setting
class GhSetting {
public:
	GhSetting();
	void load(char * path);

	bool xue;
	bool sfwin;
};
GhSetting * ghSetting;
GhSetting::GhSetting() {
	xue = false;
	sfwin = false;
}

void GhSetting::load(char * path) {
	zr_log_debug_ec(0, "load ghSetting path=%s", path);
	char buf[256];
	GetPrivateProfileString("GameHelperSetting", "xue", "false", buf, sizeof buf, path);
	this->xue = 0 == strcmp(buf, "true");
	GetPrivateProfileString("GameHelperSetting", "sfwin", "false", buf, sizeof buf, path);
	this->sfwin = 0 == strcmp(buf, "true");

}

int getMapChecksum() {
	return zr_fileVarInt_get("MapChecksum");
}

PBYTE searchMemArea(PBYTE addr, long searchLen, byte * key, long keyLen) {
	PBYTE res = 0;
	int bufLen = 0x1000000;
	byte * buf = new byte[bufLen];
	SIZE_T rlen = 0;
	MEMORY_BASIC_INFORMATION mi;
	long miLen = sizeof(mi);
	PBYTE beginAddr = addr;
	for (;; beginAddr = (PBYTE)(mi.BaseAddress) + mi.RegionSize) {
		DWORD lReturn = VirtualQueryEx(war3_porcess_handle, beginAddr, &mi,
				miLen);
		if (lReturn != miLen)
			break;

		if (MEM_COMMIT != mi.State) {
			continue;
		}

		if ((beginAddr - addr) > searchLen) {
			break;
		}

		bufLen = 0x1000000 < mi.RegionSize ? 0x1000000 : mi.RegionSize;
		if (ReadProcessMemory(war3_porcess_handle, mi.BaseAddress, buf, bufLen,
				&rlen)) {
			if (zr_memmem(buf, rlen, key, keyLen)) {
				res = (PBYTE)mi.BaseAddress;
			}
		}
	}
	delete buf;
	return res;
}

/* \Drive1\temp\buildwar3x\Engine\Source\Gxu\GxuFontUtil.cpp */
byte KEY_GxuFontUtil[] = { (byte)0x5C, (byte)0x44, (byte)0x72, (byte)0x69,
		(byte)0x76, (byte)0x65, (byte)0x31, (byte)0x5C, (byte)0x74, (byte)0x65,
		(byte)0x6D, (byte)0x70, (byte)0x5C, (byte)0x62, (byte)0x75, (byte)0x69,
		(byte)0x6C, (byte)0x64, (byte)0x77, (byte)0x61, (byte)0x72, (byte)0x33,
		(byte)0x78, (byte)0x5C, (byte)0x45, (byte)0x6E, (byte)0x67, (byte)0x69,
		(byte)0x6E, (byte)0x65, (byte)0x5C, (byte)0x53, (byte)0x6F, (byte)0x75,
		(byte)0x72, (byte)0x63, (byte)0x65, (byte)0x5C, (byte)0x47, (byte)0x78,
		(byte)0x75, (byte)0x5C, (byte)0x47, (byte)0x78, (byte)0x75, (byte)0x46,
		(byte)0x6F, (byte)0x6E, (byte)0x74, (byte)0x55, (byte)0x74, (byte)0x69,
		(byte)0x6C, (byte)0x2E, (byte)0x63, (byte)0x70, (byte)0x70 };
/*Retarded text */
byte KEY_Retarded[] = { (byte)0x52, (byte)0x65, (byte)0x74, (byte)0x61,
		(byte)0x72, (byte)0x64, (byte)0x65, (byte)0x64, (byte)0x20, (byte)0x74,
		(byte)0x65, (byte)0x78, (byte)0x74 };
/*by lovemoon03*/
byte KEY_lovemoon03[] = { (byte)0x62, (byte)0x79, (byte)0x20, (byte)0x6C,
		(byte)0x6F, (byte)0x76, (byte)0x65, (byte)0x6D, (byte)0x6F, (byte)0x6F,
		(byte)0x6E, (byte)0x30, (byte)0x33 };
/*仁德才是王道 (UTF-8) */
byte Z3_39D_KEY_4[] = { (byte)0xE4, (byte)0xBB, (byte)0x81, (byte)0xE5,
		(byte)0xBE, (byte)0xB7, (byte)0xE6, (byte)0x89, (byte)0x8D, (byte)0xE6,
		(byte)0x98, (byte)0xAF, (byte)0xE7, (byte)0x8E, (byte)0x8B, (byte)0xE9,
		(byte)0x81, (byte)0x93 };
/* 奸雄才是王者(UTF-8) */
byte Z3_39D_KEY_5[] = { (byte)0xE5, (byte)0xA5, (byte)0xB8, (byte)0xE9,
		(byte)0x9B, (byte)0x84, (byte)0xE6, (byte)0x89, (byte)0x8D, (byte)0xE6,
		(byte)0x98, (byte)0xAF, (byte)0xE7, (byte)0x8E, (byte)0x8B, (byte)0xE8,
		(byte)0x80, (byte)0x85 };
byte DOTA_JW_KEY[] = { (byte)0x7C, (byte)0x63, (byte)0x30, (byte)0x30,
		(byte)0x66, (byte)0x66, (byte)0x30, (byte)0x33, (byte)0x30, (byte)0x33,
		(byte)0xE8, (byte)0xBF, (byte)0x91, (byte)0xE5, (byte)0x8D, (byte)0xAB,
		(byte)0xE5, (byte)0x86, (byte)0x9B, (byte)0x3F, (byte)0x9B, (byte)0xA2,
		(byte)0x7C, (byte)0x72, (byte)0x20, (byte)0xE8, (byte)0x8E, (byte)0xB7,
		(byte)0xE5, (byte)0xBE, (byte)0x97, (byte)0xE4, (byte)0xBA, (byte)0x86,
		(byte)0xE8, (byte)0x83, (byte)0x9C, (byte)0xE5, (byte)0x88, (byte)0xA9,
		(byte)0xEF, (byte)0xBC, (byte)0x81, (byte)0xE8, (byte)0x8E, (byte)0xB7,
		(byte)0xE5, (byte)0x8F, (byte)0x96, (byte)0xE6, (byte)0x9C, (byte)0x80,
		(byte)0xE6, (byte)0x96, (byte)0xB0, (byte)0xE7, (byte)0x89, (byte)0x88,
		(byte)0xE6, (byte)0x9C, (byte)0xAC, (byte)0xE8, (byte)0xAF, (byte)0xB7,
		(byte)0xE7, (byte)0x99, (byte)0xBB, (byte)0xE5, (byte)0xBD, (byte)0x95,
		(byte)0xEF, (byte)0xBC, (byte)0x9A, (byte)0x20, (byte)0x77, (byte)0x77,
		(byte)0x77, (byte)0x2E, (byte)0x67, (byte)0x65, (byte)0x74, (byte)0x64,
		(byte)0x6F, (byte)0x74, (byte)0x61, (byte)0x2E, (byte)0x63, (byte)0x6F,
		(byte)0x6D };
byte DOTA_TZ_KEY[] = { (byte)0x7C, (byte)0x63, (byte)0x30, (byte)0x30,
		(byte)0x66, (byte)0x66, (byte)0x30, (byte)0x33, (byte)0x30, (byte)0x33,
		(byte)0xE8, (byte)0xBF, (byte)0x91, (byte)0xE5, (byte)0x8D, (byte)0xAB,
		(byte)0xE5, (byte)0x86, (byte)0x9B, (byte)0xE5, (byte)0x9B, (byte)0xA2,
		(byte)0x7C, (byte)0x72, (byte)0x20, (byte)0xE8, (byte)0x8E, (byte)0xB7,
		(byte)0xE5, (byte)0xBE, (byte)0x97, (byte)0xE4, (byte)0xBA, (byte)0x86,
		(byte)0xE8, (byte)0x83, (byte)0x9C, (byte)0xE5, (byte)0x88, (byte)0xA9,
		(byte)0xEF, (byte)0xBC, (byte)0x81, (byte)0xE8, (byte)0x8E, (byte)0xB7,
		(byte)0xE5, (byte)0x8F, (byte)0x96, (byte)0xE6, (byte)0x9C, (byte)0x80,
		(byte)0xE6, (byte)0x96, (byte)0xB0, (byte)0xE7, (byte)0x89, (byte)0x88,
		(byte)0xE6, (byte)0x9C, (byte)0xAC, (byte)0xE8, (byte)0xAF, (byte)0xB7,
		(byte)0xE7, (byte)0x99, (byte)0xBB, (byte)0xE5, (byte)0xBD, (byte)0x95,
		(byte)0xEF, (byte)0xBC, (byte)0x9A, (byte)0x20, (byte)0x77, (byte)0x77,
		(byte)0x77, (byte)0x2E, (byte)0x67, (byte)0x65, (byte)0x74, (byte)0x64,
		(byte)0x6F, (byte)0x74, (byte)0x61, (byte)0x2E, (byte)0x63, (byte)0x6F,
		(byte)0x6D };

PBYTE winnerScan_whenBegin(byte key_1[], long len_1, byte key_2[], long len_2,
		byte key_3[], long len_3) {
	PBYTE res = 0;

	int bufLen = 0x1000000;
	byte * buf = new byte[bufLen];
	SIZE_T rlen = 0;
	MEMORY_BASIC_INFORMATION mi;
	long miLen = sizeof(mi);

	PBYTE beginAddr = 0;

	zr_log_debug_ec(0, "z3WinnerScan_whenBegin...");
	for (;; beginAddr = (PBYTE)(mi.BaseAddress) + mi.RegionSize) {
		DWORD lReturn = VirtualQueryEx(war3_porcess_handle, beginAddr, &mi,
				miLen);
		if (lReturn != miLen)
			break;

		if (MEM_COMMIT != mi.State) {
			continue;
		}

		//bufLen = 0x1000000 < mi.RegionSize ? 0x1000000 : mi.RegionSize;
		delete buf;
		buf = new byte[mi.RegionSize];
		bufLen = mi.RegionSize;
		if (ReadProcessMemory(war3_porcess_handle, mi.BaseAddress, buf, bufLen,
				&rlen)) {
			if (zr_memmem(buf, rlen, key_1, len_1)) {
				zr_log_debug_ec(0, "found 1 len=%d at %8X", len_1,
						mi.BaseAddress);
				PBYTE addr2 = searchMemArea((PBYTE)mi.BaseAddress, 0x20000,
						key_2, len_2);
				if (addr2) {
					zr_log_debug_ec(0, "found 2 len=%d at %8X", len_2,
							mi.BaseAddress);
					if (NULL == key_3) {
						res = (PBYTE)mi.BaseAddress;
						break;
					} else {
						PBYTE addr3 = searchMemArea((PBYTE)mi.BaseAddress,
								0x20000, key_3, len_3);
						if (addr3) {
							zr_log_debug_ec(0, "found 3 len=%d at %8X", len_3,
									mi.BaseAddress);
							res = (PBYTE)mi.BaseAddress;
							break;
						}
					}
				}
			}
		}
	}
	delete buf;
	zr_log_debug_ec(0, "Scan Over");
	return res;
}

int winnerScan_whenEnd(byte key_4[], long len_4, byte key_5[], long len_5,
		PBYTE beginAddr) {
	int res= WINNER_CLIENT;

	PBYTE addr4 = searchMemArea(beginAddr, 0x20000, key_4, len_4);
	PBYTE addr5 = searchMemArea(beginAddr, 0x20000, key_5, len_5);
	if (addr4)
		return WINNER_HOST;
	if (addr5)
		return WINNER_HOST;
	return res;
}

int winTagCount1 = 0;
int winTagCount2 = 0;
PBYTE z3WinnerMemAddr = 0;
int wd_war3() {
	int mapChecksum = getMapChecksum();
	zr_log_debug_ec(0, "getMapChecksum =%d", mapChecksum);
	int win= WINNER_NONE;
	BOOL IsBeginScan = zr_fileVarBool_get("IsBeginScan");
	switch (mapChecksum) {
	case MAP_CHKSUM_Z3_39D_CREG: {
		zr_log_debug("Memscan is MAP_CHKSUM_Z3_39D_CREG");
		if (IsBeginScan) {
			Sleep(10000);
			z3WinnerMemAddr =winnerScan_whenBegin(KEY_GxuFontUtil,
					sizeof KEY_GxuFontUtil, KEY_Retarded, sizeof KEY_Retarded, 
					NULL, 0);
			zr_log_debug_ec(0, "get war3 z3 winner mem=%8X", z3WinnerMemAddr);
		} else if (z3WinnerMemAddr == 0) {
			win = WINNER_NONE;
		} else {
			win = winnerScan_whenEnd(Z3_39D_KEY_4, sizeof Z3_39D_KEY_4,
					Z3_39D_KEY_5, sizeof Z3_39D_KEY_5, z3WinnerMemAddr);
		}

		return win;
	}
	case MAP_CHKSUM_Z3_39D: {
		if (IsBeginScan) {
			Sleep(10000);
			zr_log_debug("Memscan is MAP_CHKSUM_Z3_39D");
			//			z3WinnerMemAddr = winnerScan_whenBegin(KEY_GxuFontUtil,
			//					sizeof KEY_GxuFontUtil, KEY_Retarded, sizeof KEY_Retarded, 
			//					NULL, 0);
			//			zr_log_debug_ec(0, "get war3 z3 winner mem=%8X", z3WinnerMemAddr);
			winTagCount1 = zr_memscan_count(war3_porcess_handle, 0x60000000,
					Z3_39D_KEY_4, sizeof Z3_39D_KEY_4);
			winTagCount2 = zr_memscan_count(war3_porcess_handle, 0x60000000,
					Z3_39D_KEY_5, sizeof Z3_39D_KEY_5);
		} else {
			zr_log_debug("Memscan is MAP_CHKSUM_Z3_39D");
			//			win = winnerScan_whenEnd(Z3_39D_KEY_4, sizeof Z3_39D_KEY_4,
			//					Z3_39D_KEY_5, sizeof Z3_39D_KEY_5, z3WinnerMemAddr);
			if (zr_memscan_count(war3_porcess_handle, 0x60000000, Z3_39D_KEY_4,
					sizeof Z3_39D_KEY_4) > winTagCount1) {
				win = WINNER_TN_0;
			} else if (zr_memscan_count(war3_porcess_handle, 0x60000000,
					Z3_39D_KEY_5, sizeof Z3_39D_KEY_5) > winTagCount2) {
				win = WINNER_TN_1;
			} else {
				win = WINNER_NONE;
			}
		}
		return win;
	}
	case MAP_CHKSUM_Z3_42E: {
		zr_log_debug("Memscan is MAP_CHKSUM_Z3_42E");
		if (IsBeginScan) {
			Sleep(10000);
			z3WinnerMemAddr =winnerScan_whenBegin(KEY_GxuFontUtil,
					sizeof KEY_GxuFontUtil, KEY_Retarded, sizeof KEY_Retarded, 
					NULL, 0);
			zr_log_debug_ec(0, "get war3 z3 winner mem=%8X", z3WinnerMemAddr);
		} else
			win = winnerScan_whenEnd(Z3_39D_KEY_4, sizeof Z3_39D_KEY_4,
					Z3_39D_KEY_5, sizeof Z3_39D_KEY_5, z3WinnerMemAddr);

		return win;
	}
	case MAP_CHESUM_DOTA_653_CN:
	case MAP_CHESUM_DOTA_651_CN: {
		zr_log_debug("Memscan is MAP_CHESUM_DOTA");
		if (IsBeginScan) {
			Sleep(10000);
			z3WinnerMemAddr =winnerScan_whenBegin(KEY_GxuFontUtil,
					sizeof KEY_GxuFontUtil, KEY_Retarded, sizeof KEY_Retarded, 
					NULL, 0);
			zr_log_debug_ec(0, "get war3 z3 winner mem=%8X", z3WinnerMemAddr);
		} else
			win = winnerScan_whenEnd(DOTA_JW_KEY, sizeof DOTA_JW_KEY,
					DOTA_TZ_KEY, sizeof DOTA_TZ_KEY, z3WinnerMemAddr);

		return win;
	}
	default: {
		zr_log_debug_ec(0, "MapChecksum=%d, need NOT memscan.", mapChecksum);
		return 0;
		break;
	}
	}
}

int lastWar3MemScanTime = 0;
int lastWar3MemScanRes = -1;
void startWar3MemScanListener(LPVOID lpParam) {
	SOCKADDR_IN local;
	local.sin_family=AF_INET; //Address family
	local.sin_addr.s_addr=INADDR_ANY; //Wild card IP address
	local.sin_port=htons((u_short)8497); //port to use

	//建立一个tcp socket
	SOCKET srvSock = socket(AF_INET,SOCK_STREAM,0);

	//If the socket() function fails we exit
	if (INVALID_SOCKET == srvSock) {
		zr_log_error_ec(WSAGetLastError(),
				"War3MemScanListener ServerSocket CREATE failed!");
		return;
	}

	//绑定服务器的ip地址和端口号
	if (bind(srvSock, (sockaddr*)&local, sizeof(local))!=0) {
		zr_log_error_ec(WSAGetLastError(),
				"War3MemScanListener ServerSocket BIND failed!");
		return;
	}

	//监听客户端请求，最大同时连接数10
	if (listen(srvSock, 10) != 0) {
		zr_log_error_ec(WSAGetLastError(),
				"War3MemScanListener ServerSocket LISTEN failed!");
		return;
	}

	SOCKET client;
	sockaddr_in from;
	int fromlen=sizeof(from);
	zr_log_debug("startWar3MemScanListener SUCC!!");
	while (true) {

		//接受一个连接请求，并返回一个同客户端交互的socket给变量client
		client=accept(srvSock, (struct sockaddr*)&from, &fromlen);

		//得到客户端的ip地址，并写入缓冲区
		char clientIp[64];
		sprintf(clientIp, "%s", inet_ntoa(from.sin_addr));
		zr_log_debug_ec(0, "a Connect from %s", clientIp);

		//如果是5秒内的扫描请求，直接返回, 
		int now = time(NULL);
		if (now - lastWar3MemScanTime> 5) { //否则扫描发送
			lastWar3MemScanRes = wd_war3();
			zr_log_debug_ec(0, "recently req, return direct");
			lastWar3MemScanTime = time(NULL);
		}

		// 发送内容（即：war3MemScan结果）
		char sendBuf[128];
		sprintf(sendBuf, "%d", lastWar3MemScanRes);
		zr_log_debug_ec(0, "send war3MemScanRes = %s", sendBuf);
		send(client, sendBuf, strlen(sendBuf), 0);

		//关闭连接
		closesocket(client);
	}
	zr_log_debug("startWar3MemScanListener END!!");

}



////////////////////////////////////////////////////////////
void startTcpControler(LPVOID lpParam) {
	gf_props_t props;
	gf_props_read(&props);

	char roleTag[1] = { 0x14 };
	int err = 0;

	while (true) {
		ZrSocket * zrSocket = new ZrSocket();
		if (!zrSocket->setSoTimeout(1000 * 60 * 60 * 24))
			zr_log_warn_ec(WSAGetLastError(), "setSoTimeout Failed\n");

		if (!zrSocket->setKeepAlive(true))
			zr_log_warn_ec(WSAGetLastError(), "setKeepAlive Failed\n");

		zr_log_debug_ec(0, "Tcp Controler connect to pipeServer %s:%d",
				props.pipe_host, props.pipe_tcpport);
		if (!zrSocket->connect(props.pipe_host, props.pipe_tcpport)) {
			zr_log_error_ec(WSAGetLastError(), "connect Failed\n");
			delete zrSocket;
			Sleep(2000);
			continue;
		}

		/**
		 * 向pipe host 发送初始数据包
		 * 1 个字节:角色标志 0x14 (控制监听身份)
		 * 4 个字节:自己的虚拟IP
		 */
		zrSocket->send(roleTag, 1);
		zrSocket->send((char *)props.v_ipaddr.u_bs, 4);

		int rlen = 0;
		while (true) {
			char buf[8];
			int rlen = zrSocket->recv(buf, 8);
			if (rlen <= 0)
				break;

			if (0x00 == buf[0]) {
				zr_log_debug_ec(0, "noop ctl code: %d", buf[0]);
				continue;
			}

			if (0x01 != buf[0]) {
				zr_log_debug_ec(0, "illigal ctl code: %d", buf[0]);
				continue;
			}

			char * clientId = new char[4];
			memset(clientId,0, 4);
			if (!zrSocket->blockRecv(clientId, 4)) {
				zr_log_debug_ec(GetLastError(), "ctr read req error");
				return;
			}
			
			//zr_log_debug_ec(0, "----------- %d", *(DWORD *)clientId);
			
			zr_log_debug_ec(0, "----------- %d %d %d %d", clientId[0], clientId[1], clientId[2], clientId[3]);
			zr_log_debug_ec(0, "----------- %d", *(DWORD *)clientId);
			zr_mem2file(clientId, 4, "dst_clientId");
			
			
			
			STARTUPINFO startupInfo = { 0 };
			startupInfo.cb = sizeof(startupInfo);
			PROCESS_INFORMATION processInformation;
			char tcpAdaptorPath[MAX_PATH];
			GetModuleFileName(NULL,
					tcpAdaptorPath, sizeof(tcpAdaptorPath));
			char *pos = strrchr(tcpAdaptorPath, '\\');
			*pos = '\0';
			strcat(tcpAdaptorPath, "\\gf_tcpadaptor.exe");
			zr_log_debug_ec(0, "Create process %s", tcpAdaptorPath);
			// Try to start the process
			BOOL result =:: CreateProcess(
					tcpAdaptorPath,
					NULL,
					NULL,
					NULL,
					FALSE,
					NORMAL_PRIORITY_CLASS,
					NULL,
					NULL,
					&startupInfo,
					&processInformation
			);
			if (!result) {
				zr_log_error_ec(0, "Create process failed. ");
			}
		}
		zr_log_debug("A tcp Controler connection broken.");
		delete zrSocket;
		Sleep(2000);
	}
	zr_log_debug("tcp Controler exists.");
}

////////////////////////////////////////////////////////////
void war3_smartFullWindow(LPVOID lpParam) {
	if (!ghSetting->sfwin)
		return;
	HWND war3Win = 0;

	//重复40次,每次间隔500毫米,检查寻找war3Win
	for (int i=0; i<40; i++) {
		war3Win = FindWindowA(NULL, "Warcraft III");
		if (war3Win)
			break;
		Sleep(500);
	}
	if (!war3Win) {
		zr_log_error_ec(0, "war3 hwnd is not found!!");
		return;
	}
	long style = GetWindowLongA(war3Win, GWL_STYLE);
	style = style ^ WS_OVERLAPPEDWINDOW;
	if (!SetWindowLongA(war3Win, GWL_STYLE, style)) {
		zr_log_error_ec(GetLastError(), "set war3 window failed");
		return;
	}

	/* 最大化屏幕 */
	HDC dc= CreateDC("DISPLAY",NULL,NULL,NULL);
	int width = GetDeviceCaps(dc, HORZRES);
	int height = GetDeviceCaps(dc, VERTRES);
	DeleteDC(dc);
	int left = 0;
	int top = 0;
	SetWindowPos(war3Win, 0, left, top, width, height, 0);

	zr_log_error_ec(0, "set war3 window OK!");
}

////////////////////////////////////////////////////////////
void udp_dig(gf_props_t * p_props, u_short port, bool is_open) {
	int err = 0;

	u_short nport = htons(port);
	char *p_nport = (char *)&nport;

	// sockaddr_in - client
	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = nport;
	client.sin_addr.s_addr = INADDR_ANY;

	// sockaddr_in - server
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(p_props->pipe_udpport);
	server.sin_addr.s_addr = inet_addr(p_props->pipe_host);

	// build socket 
	SOCKET sock;
	sock=socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == sock) {
		printf("socket create failed.\r\n", WSAGetLastError());
		return;
	}

	err = bind(sock, (struct sockaddr*)&client, sizeof client);
	if (SOCKET_ERROR == err) {
		printf("Udp socket bind error[%d].\r\n", GetLastError());
		return;
	}

	// data package 
	char buf[9] = "\0";
	if (is_open) {
		* (buf + 0) = 'd';
		* (buf + 1) = 'i';
		* (buf + 2) = 'g';
	} else {
		* (buf + 0) = 'u';
		* (buf + 1) = 'd';
		* (buf + 2) = 'g';
	}
	* (buf + 3) = p_props->v_ipaddr.u_bs[0];
	* (buf + 4) = p_props->v_ipaddr.u_bs[1];
	* (buf + 5) = p_props->v_ipaddr.u_bs[2];
	* (buf + 6) = p_props->v_ipaddr.u_bs[3];
	* (buf + 7) = *(p_nport + 0);
	* (buf + 8) = *(p_nport + 1);

	// send
	err = sendto(sock, buf, sizeof buf, 0, (struct sockaddr*)&server,
			sizeof server);
	if (SOCKET_ERROR == err) {
		printf("Udp send error.\r\n");
	}

	// close & clean
	closesocket(sock);
}

/**
 * UDP打洞数据包，格式如下：
 * 3 个字节：{'d', 'i', 'g'}
 * 4 个字节：自身的虚拟ip地址，网络格式
 * 2 个字节：自身监听udp端口号，网络格式
 */
void udp_dig_main(gf_props_t * p_props, boolean is_open) {
	int err = 0;

	u_short bgn_port = p_props->udpdig_port_bgn;
	u_short end_port = p_props->udpdig_port_end;

	for (int port=bgn_port; port<=end_port; port++) {
		udp_dig(p_props, port, is_open);
	}

}

void start_prepare(bool is_udp_open) {
	// 读取临时配置
	gf_props_t props;
	gf_props_read(&props);
	gf_props_debug(&props);

	// udp打洞
	udp_dig_main(&props, is_udp_open);

	// 启动tcp adaptor线程
	if (1 == props.has_tcp) {
		DWORD far threadID;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startTcpControler, NULL, 0, &threadID);
	} else
		zr_log_debug("No TCP.");
}

//////////////////////////////////////////////////////////////////////////////


#define arrayof(x)      (sizeof(x)/sizeof(x[0]))

//////////////////////////////////////////////////////////////////////////////
//
void PrintUsage(void) {
	printf("Usage:\n"
		"    gf_startgame.exe [options] [command line]\n"
		"Options:\n"
		"    /d:file.dll   : Start the process with file.dll.\n"
		"    /p:path       : Specify path to detoured.dll.\n"
		"    /?            : This help screen.\n");
}

//////////////////////////////////////////////////////////////////////////////
//
//  This code verifies that the named DLL has been configured correctly
//  to be imported into the target process.  DLLs must export a function with
//  ordinal #1 so that the import table touch-up magic works.
//
static BOOL CALLBACK ExportCallback(PVOID pContext, ULONG nOrdinal, PCHAR pszSymbol,
		PVOID pbTarget) {
	(void)pContext;
	(void)pbTarget;
	(void)pszSymbol;

	if (nOrdinal == 1) {
		printf("nOrdinal=%d\r\n", nOrdinal);
		*((BOOL *)pContext) = TRUE;
	}
	return TRUE;
}

BOOL DoesDllExportOrdinal1(PCHAR pszDllPath) {
	HMODULE hDll= LoadLibraryEx(pszDllPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
	if (hDll == NULL) {
		printf("gf_startgame.exe: LoadLibraryEx(%s) failed with error %d.\n",
				pszDllPath, GetLastError());
		return FALSE;
	}

	BOOL validFlag= FALSE;
	DetourEnumerateExports(hDll, &validFlag, ExportCallback);
	FreeLibrary(hDll);
	return validFlag;
}

#define GS_MISSION_NONE 0
#define GS_MISSION_GAMESTART 1
#define GS_MISSION_SPEED 2
#define GS_MISSION_HARD 3

int gs_mission= GS_MISSION_NONE;
bool is_terminated_by_mh = false;
///////////////////////////////////////////
int CDECL __main(int argc, char ** argv) {
	//读取GameHelperSetting
	char ghSettingPath[MAX_PATH];
	GetModuleFileName(NULL,
			ghSettingPath, sizeof(ghSettingPath));
	char *pos = strrchr(ghSettingPath, '\\');
	*pos = '\0';
	strcat(ghSettingPath, "\\..\\conf\\ghSetting.ini");
	ghSetting = new GhSetting();
	ghSetting->load(ghSettingPath);
	zr_log_debug_ec(0, "xue=%d", ghSetting->xue);

	//Logger Setting
	zr_log_set_target(ZR_LOG_TG_FILE);
	char logpath[MAX_PATH];
	GetTempPath(sizeof logpath, logpath);
	strcat(logpath, "gf.log");
	zr_log_set_filepath(logpath);

	if ((3 == argc) && (strcmp(argv[1], "ayssssmh") == 0)) {
		char war3_dirpath[MAX_PATH];
		strcpy(war3_dirpath, argv[2]);
		if (ac_wc3_found_aysMH_final(war3_dirpath)) {
			printf("Found ayssssMH final edition.");
		} else {
			printf("No Found any MH.");
		}
		return 0;
	}
	// Segment: speed testing
	if ((3 == argc) && (strcmp(argv[1], "speed") == 0)) {
		gs_mission = GS_MISSION_SPEED;
		char * rooms_path = argv[2];
		spd_test(rooms_path);
		return 0;
	}

	if ((2 == argc) && (strcmp(argv[1], "hard") == 0)) {
		gs_mission = GS_MISSION_HARD;
		save_hard_info();
		return 0;
	}

	/////////////////////////////////////
	// Start Game
	//////////////////////////////////////


	// parse args
	gs_mission = GS_MISSION_GAMESTART;
	BOOLEAN fNeedHelp= FALSE;
	PCHAR pszDllPath= NULL;
	PCHAR pszDetouredDllPath= NULL;

	int arg = 1;
	for (; arg < argc && (argv[arg][0] == '-' || argv[arg][0] == '/'); arg++) {

		CHAR *argn = argv[arg] + 1;
		CHAR *argp = argn;
		while (*argp && *argp != ':')
			argp++;
		if (*argp == ':')
			*argp++ = '\0';

		switch (argn[0]) {
		case 'd': // Set DLL Name
		case 'D':
			pszDllPath = argp;
			break;

		case 'p': // Path to Detoured.dll
		case 'P':
			pszDetouredDllPath = argp;
			break;

		case '?': // Help
			fNeedHelp = TRUE;
			break;

		default:
			fNeedHelp = TRUE;
			printf("gf_startgame.exe: Bad argument: %s\n", argv[arg]);
			break;
		}
	}

	if (arg >= argc) {
		fNeedHelp = TRUE;
	}

	if (pszDllPath == NULL) {
		fNeedHelp = TRUE;
	}

	if (fNeedHelp) {
		PrintUsage();
		return 9001;
	}

	/////////////////////////////////////////////////////////// Validate DLLs.
	//
	CHAR szDllPath[1024];
	CHAR szDetouredDllPath[1024];
	PCHAR pszFilePart= NULL;

	if (!GetFullPathName(pszDllPath, arrayof(szDllPath), szDllPath, &pszFilePart)) {
		printf("gf_startgame.exe: Error: %s is not a valid path name..\n",
				pszDllPath);
		return 9002;
	}
	if (!DoesDllExportOrdinal1(pszDllPath)) {
		printf(
				"gf_startgame.exe: Error: %s does not export function with ordinal #1.\n",
				pszDllPath);
		return 9003;
	}

	if (pszDetouredDllPath != NULL) {
		if (!GetFullPathName(pszDetouredDllPath,
				arrayof(szDetouredDllPath),
				szDetouredDllPath,
				&pszFilePart)) {
			printf("gf_startgame.exe: Error: %s is not a valid path name.\n",
					pszDetouredDllPath);
			return 9004;
		}
		if (!DoesDllExportOrdinal1(pszDetouredDllPath)) {
			printf(
					"gf_startgame.exe: Error: %s does not export function with ordinal #1.\n",
					pszDetouredDllPath);
			return 9005;
		}
	} else {
		HMODULE hDetouredDll = DetourGetDetouredMarker();
		GetModuleFileName(hDetouredDll,
				szDetouredDllPath, arrayof(szDetouredDllPath));
#if 0
		if (!SearchPath(NULL, "detoured.dll", NULL,
						arrayof(szDetouredDllPath),
						szDetouredDllPath,
						&pszFilePart)) {
			printf("gf_startgame.exe: Couldn't find Detoured.DLL.\n");
			return 9006;
		}
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	CHAR szCommand[2048];
	CHAR szExe[1024];
	CHAR szFullExe[1024] = "\0";
	PCHAR pszFileExe= NULL;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	szCommand[0] = L'\0';

#ifdef _CRT_INSECURE_DEPRECATE
	strcpy_s(szExe, sizeof(szExe), argv[arg]);
#else
	strcpy(szExe, argv[arg]);
#endif
	for (; arg < argc; arg++) {
		if (strchr(argv[arg], ' ') != NULL || strchr(argv[arg], '\t') != NULL) {
#ifdef _CRT_INSECURE_DEPRECATE
			strcat_s(szCommand, sizeof(szCommand), "\"");
			strcat_s(szCommand, sizeof(szCommand), argv[arg]);
			strcat_s(szCommand, sizeof(szCommand), "\"");
#else
			strcat(szCommand, "\"");
			strcat(szCommand, argv[arg]);
			strcat(szCommand, "\"");
#endif
		}
		else {
#ifdef _CRT_INSECURE_DEPRECATE
			strcat_s(szCommand, sizeof(szCommand), argv[arg]);
#else
			strcat(szCommand, argv[arg]);
#endif
		}

		if (arg + 1 < argc) {
#ifdef _CRT_INSECURE_DEPRECATE
			strcat_s(szCommand, sizeof(szCommand), " ");
#else
			strcat(szCommand, " ");
#endif
		}
	}
	printf("gf_startgame.exe: Starting: `%s'\n", szCommand);
	printf("gf_startgame.exe:   with `%s'\n\n", szDllPath);
	printf("gf_startgame.exe:   marked by `%s'\n\n", szDetouredDllPath);
	fflush(stdout);

	DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;

	SetLastError(0);

	////////////////////////////////////////////////////////////////////////
	// start_prepare
	////////////////////////////////////////////////////////////////////////
	start_prepare(true);
	Sleep(1000);

	////////////////////////////////////////////////////////////////////////
	// 进入游戏路径，否则CS在进入服务器的的时候会出错
	////////////////////////////////////////////////////////////////////////
	game_dir [0] = '\0';
	strcpy(game_dir, szExe);
	char * idx = strrchr(game_dir, '\\');
	* idx = '\0';
	zr_log_debug_ec(0, "cd %s", game_dir);
	SetCurrentDirectory(game_dir);

	////////////////////////////////////////////////////////////////////////
	// 判断是否是War3 
	// war3exe_exist_OK
	char path[MAX_PATH];
	sprintf(path, "%s\\war3.exe", game_dir);
	if (ZR_FILE_SUCC == zr_fexist(path)) {
		G_curGame = GAME_ID_WAR3;
	}
	zr_fileVarStr_set("gameDir", game_dir);
	//printf("%d\r\n", G_curGame);

	// 设置replaysDirPath
	{
		char pathBuf[MAX_PATH];
		GetModuleFileName(NULL,
				pathBuf, MAX_PATH);
		char *pos = strrchr(pathBuf, '\\');
		*pos = '\0';
		pos = strrchr(pathBuf, '\\');
		*pos = '\0';
		strcat(pathBuf, "\\w3gReplays");
		zr_fileVarStr_set("replaysDirPath", pathBuf);
	}

	if (GAME_ID_WAR3 == G_curGame) {
		if (ghSetting->sfwin) {
			if (!strstr(szCommand, "-window")) {
				strcat(szCommand, " -window");
			}
		}

		//对付lpk.dll MH, ac_wc3_check_del_lpk
		if (ac_wc3_check_del_lpk(game_dir)) {
			char * msg = "发现 MH 程序，而您正处在公平竞技房间。\r\n为了竞技游戏的公正性，请关闭MH软件，或到其他非限制房间游戏。\r\n如你没有故意运行MH程序，则很可能您的游戏程序曾经受到了MH软件的改写。请重新下载魔兽补丁。";
			MessageBox(NULL, msg, "MH警告", NULL);
			return 0;
			//	exit(0);
		}

	}

	////////////////////////////////////////////////////////////////////////
	// 判断是否是War3 120e
	// 120e 防秒退预处理
	////////////////////////////////////////////////////////////////////////
	bool is_war3_120e = ac_wc3_check_ver_120e(game_dir);
	if (is_war3_120e) {
		//		ac_wc3secout_120e_begin(game_dir);
	}
	zr_log_debug_ec(0, "ac_wc3_check_ver_120e res=%d", is_war3_120e);

	////////////////////////////////////////////////////////////////////////
	// start_game process
	////////////////////////////////////////////////////////////////////////

	SearchPath(NULL, szExe, ".exe", arrayof(szFullExe), szFullExe, &pszFileExe);
	if (!DetourCreateProcessWithDll(szFullExe[0] ? szFullExe : NULL, szCommand,
					NULL, NULL, TRUE, dwFlags, NULL, NULL,
					&si, &pi, szDetouredDllPath, szDllPath, NULL)) {
		printf("gf_startgame.exe: DetourCreateProcess gf_startgame failed: %d\n", GetLastError());
		return 9007;
	}

	ResumeThread(pi.hThread);

	zr_fileVarBool_set("fightBegin", false);
	zr_fileVarBool_set("war3WinnerScan", false);

	////////////////////////////////////////////////////////////////////////
	// 判断是否war3 MH
	////////////////////////////////////////////////////////////////////////
	ac_hct_param_t ac_hct_param;
	ac_hct_param.type = AC_GAMETYPE_NONE;
	if (is_war3_120e) {

		//准备参数
		//启动ac_hack_check_thread检查Thread
		ac_hct_param.type = AC_GAMETYPE_WC3120e;
		ac_hct_param.war3_process_handle = pi.hProcess;
		strcpy(ac_hct_param.war3_dirpath, game_dir);
		ac_start_hack_check_thread(&ac_hct_param);

		//保留war3_porcess_handle
		war3_porcess_handle = ac_hct_param.war3_process_handle;

		//防秒退,内存改写
		bool secout_res = ac_wc3secout_120e_dealwith(pi.hProcess);
		zr_log_debug_ec(0, "ac_wc3secoutA_120e_dealwith %s", secout_res ? "OK": "FAIL");
	}
	if (GAME_ID_WAR3 == G_curGame) {
		// war3 胜负检查线程
		DWORD far threadID;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startWar3MemScanListener, NULL, 0, &threadID);
	}

	//显血
	if (GAME_ID_WAR3 == G_curGame) {
		if (ghSetting->xue) {
			LPVOID poses[] = {(LPVOID)0x6F17F133,(LPVOID)0x6F17F150}; //只适用于1.20e
			byte v = 235;
			byte o = 0;
			DWORD wLen, rLen;
			for(int i=0; i< 2; i++) {
				zr_log_error_ec(0, "Xue scan changing.... %8X", poses[i]);
				if (!ReadProcessMemory(war3_porcess_handle, poses[i], &o, 1, &rLen)) {
					zr_log_error_ec(GetLastError(), "Xue: ReadProcessMemory	error at %8X");
					continue;
				}
				if (o != 117) {
					zr_log_error_ec(GetLastError(), "Xue: Not 117 at %8X", poses[i]);
					continue;
				}
				if (!WriteProcessMemory(war3_porcess_handle, poses[i], &v, 1, &wLen)) {
					zr_log_error_ec(GetLastError(), "Xue: WriteProcessMemory error at %8X");
					continue;
				}
				zr_log_error_ec(0, "Xue OK!");
			}
		}
	}

	//智能全屏窗
	if(GAME_ID_WAR3 == G_curGame) {
		DWORD far threadID;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)war3_smartFullWindow, NULL, 0, &threadID);
		//war3_smartFullWindow();
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	if (is_terminated_by_mh) {
		char * msg = "发现 MH 程序，而您正处在公平竞技房间。\r\n为了竞技游戏的公正性，请关闭MH软件，或到其他非限制房间游戏。\r\n如你没有故意运行MH程序，则很可能您的游戏程序曾经受到了MH软件的改写。请重新下载魔兽补丁。";
		MessageBox(NULL, msg, "MH警告", NULL);
		//	exit(0);
	}

	//	ac_wc3secout_120e_end(game_dir);
	if (GAME_ID_WAR3 == G_curGame) {
		ac_stop_hack_check_thread();
	}

	start_prepare(false);

	DWORD dwResult = 0;
	if (!GetExitCodeProcess(pi.hProcess, &dwResult)) {
		zr_log_error_ec(GetLastError(),"gf_startgame.exe: GetExitCodeProcess failed");
		dwResult = 9008;
	}

	return dwResult;
}

//////////////////////////////////////////////////////////////////
void sendAppExist() {
	zr_log_debug("send exist game pkg");
	int err = 0;
	/**
	 * 连接XPCom启动的游戏结束监听线程
	 */
	SOCKET sock=socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN soAddr;
	soAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	soAddr.sin_family=AF_INET;
	soAddr.sin_port=htons(9089);
	zr_log_debug("connect to game exist listener socket.");
	err = connect(sock, (SOCKADDR*)&soAddr, sizeof(SOCKADDR));
	if (SOCKET_ERROR == err) {
		zr_log_debug_ec(err, "connect failed.");
		return;
	}
	// close & clean
	closesocket(sock);
}
//////////////////////////////////////////////////////////////////
int CDECL main(int argc, char **argv) {
	if (!zr_sock_startup()) {
		zr_log_error("app start failed. zr_sock_startup failed.");
		return 0;
	}

	int res = __main(argc, argv);
	if (GS_MISSION_GAMESTART == gs_mission)
		sendAppExist();

	zr_sock_cleanup();
	return res;
}

//
///////////////////////////////////////////////////////////////// End of File.
