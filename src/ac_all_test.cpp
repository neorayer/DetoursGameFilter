#include <iostream> 
#include <fstream> 
#include <stdlib.h>
#include <windows.h>

#include <zr_file.h>

#include "winsdk/Psapi.h"

#include "ac_all.h"

using namespace std;

bool is_terminated_by_mh = false;
int main(void) {

	//	bool res = ac_check_wc3mh_120e("c:\\temp\\delme2\\game1.dll");

	//	char * MH_WN_mh120e5 = "W3XMH";
	//	HWND hwnd= FindWindow( NULL, MH_WN_mh120e5);
	//	if (NULL == hwnd) {
	//		printf("no found window");
	//		return 0;
	//	}
	//	printf("find %d\r\n", hwnd);
	//
	//	DWORD pid;
	//	GetWindowThreadProcessId(hwnd, &pid);
	//	printf("find pid %d\r\n", pid);
	//
	//	//	char * msg = "发现 MH 程序，而您正处在公平竞技房间。\r\n为了竞技游戏的公正性，请关闭MH软件，或到其他房间游戏。";
	//	//	MessageBox(NULL, msg, "MH警告", NULL);

	//	ac_start_hack_check_thread();

//	HWND hwar3= FindWindow(NULL,"Warcraft III");
//	printf("%d\r\n", hwar3);
//
//	HANDLE hcurrent=GetCurrentProcess();
//	HANDLE hToken;
//	BOOL bret=OpenProcessToken(hcurrent, 40, &hToken);
//
//	LUID luid;
//	bret=LookupPrivilegeValue(NULL,"SeDebugPrivilege",&luid);
//	TOKEN_PRIVILEGES NewState, PreviousState;
//	DWORD ReturnLength;
//	NewState.PrivilegeCount =1;
//	NewState.Privileges[0].Luid =luid;
//	NewState.Privileges[0].Attributes=2;
//	bret=AdjustTokenPrivileges(hToken, FALSE, &NewState, 28, &PreviousState,
//			&ReturnLength);
//
//	DWORD PID, TID;
//	TID = GetWindowThreadProcessId(hwar3, &PID);
//
//	HANDLE
//			hopen=
//					OpenProcess( PROCESS_ALL_ACCESS|PROCESS_TERMINATE|PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE,FALSE,PID);
//
////	char memTag1;
////	SIZE_T readLen1 = 0;
////	ReadProcessMemory(hopen,(LPVOID) 0x6F406B4F, &memTag1, sizeof memTag1,&readLen1 );
////	
////	MessageBox(NULL, "Open aysMH please", "Open aysMH", MB_OK);
////	
////	char memTag2;
////	SIZE_T readLen2 = 0;
////	ReadProcessMemory(hopen,(LPVOID) 0x6F406B4F, &memTag2, sizeof memTag2,&readLen2 );
////
////	if (memTag1 != memTag2) {
////		printf("%x!=%x\r\n",memTag1, memTag2);
////	}
//
//#define BUF_LEN 0x10000
//#define START_P  0x6F4069F0 
//	
//	char buf1[BUF_LEN];
//	SIZE_T readLen1 = 0;
//	ReadProcessMemory(hopen, (LPVOID) START_P, buf1, sizeof buf1,
//			&readLen1);
//
//	MessageBox(NULL, "Open aysMH please", "Open aysMH", MB_OK);
//
//	char buf2[BUF_LEN];
//	SIZE_T readLen2 = 0;
//	ReadProcessMemory(hopen, (LPVOID) START_P, buf2, sizeof buf2,
//			&readLen2);
//
//	for(int i=0; i<BUF_LEN; i++) {
//		if (buf1[i] == buf2[i])
//			continue;
//		printf("%0x %x!=%x\r\n", i, buf1[i], buf2[i]);		
//	}
//
	wc3gamedll_keynode_t kns[4];
	kns[0].pos = 4221775;
	kns[0].val = 0x09;
	kns[1].pos = 4221776;
	kns[1].val = 0x90;
	kns[2].pos = 4221780;
	kns[2].val = 0x09;
	kns[3].pos = 4221781;
	kns[3].val = 0x90;

	
	char *gamedll_path = "c:\\wc3\\game.dll";
	//char *gamedll_path = "c:\\temp\\delme1\\1.txt";
	ifstream stream(gamedll_path, fstream::in | fstream::binary);
	for (int i=0; i<4; i++) {
		stream.seekg(kns[i].pos, ios::beg);
//		char data[1];
//		data[0] = 1;
//		stream.get
//		stream.get(data, 1);
//		printf( "%d\r\n", data[0]);

		char c;
		stream.get(c);
		printf( "%d\r\n", c);
	}
	stream.close();

	return 0;
}