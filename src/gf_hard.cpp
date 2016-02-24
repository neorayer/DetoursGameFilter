#include   <stdio.h>  
#include   <tchar.h>  
#include   <locale.h>  
#include   <windows.h>  

#include   <list>  
#include   <string>  

#ifdef   _UNICODE  
typedef std::wstring string;
#else  
typedef std::string string;
#endif  

void video_card_info(void) {
	HKEY keyServ=   NULL;
	HKEY keyEnum=   NULL;
	HKEY key=   NULL;
	HKEY key2=   NULL;
	DWORD error = 0;
	LPCTSTR api=   NULL;
	DWORD type, count, size, i, iServ;
	std::list <string> save;

	setlocale(LC_ALL, ".ACP");

	error = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
			TEXT ("SYSTEM\\CurrentControlSet\\Services"),
			0, KEY_READ, &keyServ);

	error = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
			TEXT ("SYSTEM\\CurrentControlSet\\Enum"),
			0, KEY_READ, &keyEnum);

	i = 0;
	for (iServ=0;; ++iServ) {
		TCHAR name [0x200];
		size = 0x200;
		error = RegEnumKeyEx (keyServ, iServ, name, &size,
				NULL, NULL, NULL, NULL);

		if (error == ERROR_NO_MORE_ITEMS)
			break;

		error = RegOpenKeyEx (keyServ, name, 0, KEY_READ, &key);

		size = 0x200;
		error = RegQueryValueEx (key, TEXT ("Group"),
				0, &type, (LPBYTE)name, &size);

		if (error == ERROR_FILE_NOT_FOUND) { //     键不存在  
			RegCloseKey(key);
			continue;
		};

		if (_tcscmp(TEXT ("Video"), name) != 0) { //     不是显卡的内核驱动  
			RegCloseKey(key);
			continue;
		};

		error = RegOpenKeyEx (key, TEXT ("Enum"),
				0, KEY_READ, &key2);

		RegCloseKey(key);
		key = key2;

		size = sizeof (count);
		error = RegQueryValueEx (key, TEXT ("Count"),
				0, &type, (LPBYTE)&count, &size);

		for (i=0; i<count; ++i) {
			TCHAR sz [0x200];
			TCHAR name [0x40];

			wsprintf (name, TEXT ("%d"), i);
			size = sizeof (sz);
			error = RegQueryValueEx (key, name,
					0, &type, (LPBYTE)sz, &size);

			error = RegOpenKeyEx (keyEnum, sz,
					0, KEY_READ, &key2);

			size = sizeof (sz);
			error = RegQueryValueEx (key2, TEXT ("FriendlyName"),
					0, &type, (LPBYTE)sz, &size);

			if (error == ERROR_FILE_NOT_FOUND) {
				size = sizeof (sz);
				error = RegQueryValueEx (key2, TEXT ("DeviceDesc"),
						0, &type, (LPBYTE)sz, &size);
			};

			save.push_back(sz);
			RegCloseKey(key2);
			key2 = NULL;
		};
		RegCloseKey(key);
		key = NULL;
	};

	RegCloseKey(keyEnum);
	RegCloseKey(keyServ);

	if (true) {
		std::list <string>::iterator it, end;
		it = save.begin();
		end = save.end();

		//		_tprintf(TEXT("您的系统上共有   %u   块显卡:   \n\n"), save.size ());
		for (i=0; it!=end; ++it)
			_tprintf(TEXT ("显卡%d:= %s\n"), i++, it->c_str ());
	}

	failed: if (keyServ)
		RegCloseKey(keyServ);
	if (keyEnum)
		RegCloseKey(keyEnum);
	if (key)
		RegCloseKey(key);
	if (key2)
		RegCloseKey(key2);
}

int save_hard_info() {
	char path[MAX_PATH];
	GetTempPath(MAX_PATH, path);
	strcat(path, "gf_hard.ini");
	
	MEMORYSTATUS memStat;
	GlobalMemoryStatus(&memStat);
	HKEY hKey;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
	DWORD BufferSize = 8192;
	DWORD cbData;
	DWORD dwRet;

	PPERF_DATA_BLOCK PerfData = (PPERF_DATA_BLOCK) malloc(BufferSize);
	cbData = BufferSize;

	dwRet= RegQueryValueEx(hKey, "ProcessorNameString", 0, 0, (LPBYTE) PerfData,&cbData);

	RegCloseKey(hKey);
	
	FILE * fp = fopen(path, "w");
	fprintf(fp, "[HARD INFO]\r\n");
	fprintf(fp, "mem=%dM\r\n", memStat.dwTotalPhys / 1024 / 1024);
	fprintf(fp, "cpu=%s\r\n", PerfData);
	fclose(fp);
	
	return 0;
}