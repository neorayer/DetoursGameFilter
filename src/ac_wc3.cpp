#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winuser.h>
#include <iostream> 
#include <fstream> 

#include <zr_logger.h>
#include <zr_file.h>

#include "winsdk/Psapi.h"
#include "ac_all.h"

#include "gf_head.h"

using namespace std;

//
//AntiCheaterWc3MH::AntiCheaterWc3MH(){
//	
//}
//AntiCheaterWc3MH::~AntiCheaterWc3MH(){
//	
//}
//
//void AntiCheaterWc3MH::check(){
//	
//}


//bool ac_check_wc3mh_120e(char * gamedll_path) {
//	wc3gamedll_keynode_t kns[92];
//
//	kns[0].pos = 1235994;
//	kns[0].val = 0x74;
//	kns[1].pos = 1235995;
//	kns[1].val = 0x08;
//	kns[2].pos = 1236058;
//	kns[2].val = 0x74;
//	kns[3].pos = 1236059;
//	kns[3].val = 0x08;
//	kns[4].pos = 1306371;
//	kns[4].val = 0x75;
//	kns[5].pos = 1348008;
//	kns[5].val = 0x01;
//	kns[6].pos = 1348832;
//	kns[6].val = 0x85;
//	kns[7].pos = 1348835;
//	kns[7].val = 0x84;
//	kns[8].pos = 1470046;
//	kns[8].val = 0x85;
//	kns[9].pos = 1470047;
//	kns[9].val = 0xC0;
//	kns[10].pos = 1470048;
//	kns[10].val = 0x0F;
//	kns[11].pos = 1470049;
//	kns[11].val = 0x84;
//	kns[12].pos = 1470050;
//	kns[12].val = 0xBB;
//	kns[13].pos = 1470051;
//	kns[13].val = 0x03;
//	kns[14].pos = 1470052;
//	kns[14].val = 0x00;
//	kns[15].pos = 1470053;
//	kns[15].val = 0x00;
//	kns[16].pos = 1506826;
//	kns[16].val = 0x85;
//	kns[17].pos = 1506828;
//	kns[17].val = 0x74;
//	kns[18].pos = 1506829;
//	kns[18].val = 0x78;
//	kns[19].pos = 1561832;
//	kns[19].val = 0x6A;
//	kns[20].pos = 1561833;
//	kns[20].val = 0x00;
//	kns[21].pos = 1561834;
//	kns[21].val = 0x50;
//	kns[22].pos = 1561835;
//	kns[22].val = 0x56;
//	kns[23].pos = 1561836;
//	kns[23].val = 0xE8;
//	kns[24].pos = 1561837;
//	kns[24].val = 0x5F;
//	kns[25].pos = 1561838;
//	kns[25].val = 0x35;
//	kns[26].pos = 1561839;
//	kns[26].val = 0x12;
//	kns[27].pos = 1569075;
//	kns[27].val = 0x75;
//	kns[28].pos = 1569104;
//	kns[28].val = 0x75;
//	kns[29].pos = 1757180;
//	kns[29].val = 0xC3;
//	kns[30].pos = 1757181;
//	kns[30].val = 0x90;
//	kns[31].pos = 1833662;
//	kns[31].val = 0x75;
//	kns[32].pos = 2754501;
//	kns[32].val = 0x8B;
//	kns[33].pos = 2754502;
//	kns[33].val = 0x59;
//	kns[34].pos = 2754503;
//	kns[34].val = 0x14;
//	kns[35].pos = 2754504;
//	kns[35].val = 0x8B;
//	kns[36].pos = 2754505;
//	kns[36].val = 0x49;
//	kns[37].pos = 2754506;
//	kns[37].val = 0x10;
//	kns[38].pos = 2754851;
//	kns[38].val = 0x8B;
//	kns[39].pos = 2754852;
//	kns[39].val = 0x42;
//	kns[40].pos = 2754853;
//	kns[40].val = 0x14;
//	kns[41].pos = 2754854;
//	kns[41].val = 0x8B;
//	kns[42].pos = 2754855;
//	kns[42].val = 0x52;
//	kns[43].pos = 2754856;
//	kns[43].val = 0x10;
//	kns[44].pos = 4221775;
//	kns[44].val = 0x49;
//	kns[45].pos = 4221776;
//	kns[45].val = 0x04;
//	kns[46].pos = 4221780;
//	kns[46].val = 0x49;
//	kns[47].pos = 4221781;
//	kns[47].val = 0x08;
//	kns[48].pos = 4467904;
//	kns[48].val = 0x0F;
//	kns[49].pos = 4467905;
//	kns[49].val = 0x84;
//	kns[50].pos = 4467906;
//	kns[50].val = 0xDC;
//	kns[51].pos = 4467907;
//	kns[51].val = 0x00;
//	kns[52].pos = 4467908;
//	kns[52].val = 0x00;
//	kns[53].pos = 4467909;
//	kns[53].val = 0x00;
//	kns[54].pos = 4469621;
//	kns[54].val = 0x74;
//	kns[55].pos = 4469622;
//	kns[55].val = 0x1E;
//	kns[56].pos = 4564545;
//	kns[56].val = 0x0F;
//	kns[57].pos = 4564546;
//	kns[57].val = 0x84;
//	kns[58].pos = 4564547;
//	kns[58].val = 0x60;
//	kns[59].pos = 4564548;
//	kns[59].val = 0x01;
//	kns[60].pos = 4564549;
//	kns[60].val = 0x00;
//	kns[61].pos = 4564550;
//	kns[61].val = 0x00;
//	kns[62].pos = 4581278;
//	kns[62].val = 0x0F;
//	kns[63].pos = 4581279;
//	kns[63].val = 0x84;
//	kns[64].pos = 4581280;
//	kns[64].val = 0x26;
//	kns[65].pos = 4581281;
//	kns[65].val = 0x01;
//	kns[66].pos = 4581282;
//	kns[66].val = 0x00;
//	kns[67].pos = 4581283;
//	kns[67].val = 0x00;
//	kns[68].pos = 4613415;
//	kns[68].val = 0x74;
//	kns[69].pos = 4613416;
//	kns[69].val = 0x0F;
//	kns[70].pos = 4633176;
//	kns[70].val = 0x0F;
//	kns[71].pos = 4633177;
//	kns[71].val = 0x84;
//	kns[72].pos = 4633178;
//	kns[72].val = 0x91;
//	kns[73].pos = 4633179;
//	kns[73].val = 0x00;
//	kns[74].pos = 4633180;
//	kns[74].val = 0x00;
//	kns[75].pos = 4633181;
//	kns[75].val = 0x00;
//	kns[76].pos = 4854176;
//	kns[76].val = 0x74;
//	kns[77].pos = 4854177;
//	kns[77].val = 0x0F;
//	kns[78].pos = 5554367;
//	kns[78].val = 0x0F;
//	kns[79].pos = 5554368;
//	kns[79].val = 0x84;
//	kns[80].pos = 5554369;
//	kns[80].val = 0xF7;
//	kns[81].pos = 5554370;
//	kns[81].val = 0x00;
//	kns[82].pos = 5554371;
//	kns[82].val = 0x00;
//	kns[83].pos = 5554372;
//	kns[83].val = 0x00;
//	kns[84].pos = 5600254;
//	kns[84].val = 0x0F;
//	kns[85].pos = 5600255;
//	kns[85].val = 0x84;
//	kns[86].pos = 5600256;
//	kns[86].val = 0x61;
//	kns[87].pos = 5600257;
//	kns[87].val = 0x01;
//	kns[88].pos = 5600258;
//	kns[88].val = 0x00;
//	kns[89].pos = 5600259;
//	kns[89].val = 0x00;
//	kns[90].pos = 5628252;
//	kns[90].val = 0x74;
//	kns[91].pos = 5628253;
//	kns[91].val = 0x4A;
//
//	HANDLE
//			fh= CreateFile(gamedll_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
//	if (fh < 0) {
//		//		printf("Open fi%d %d\r\n", GetLastError(), fh);
//	}
//
//	CloseHandle(fh);
//	return false;
//}

bool ac_war3_found_jiajiaMH(HANDLE war3_porcess_handle) {
	LPCVOID ScanAddr[] = { (LPCVOID)0x6F1ACFFD, (LPCVOID)0x6F17D4E8,
			(LPCVOID)0x6F17D4E9, (LPCVOID)0x6F5EE22C, (LPCVOID)0x6F4A7999,
			(LPCVOID)0x6F4A799A, (LPCVOID)0x6F321CC4, (LPCVOID)0x6F23D4FC,
			(LPCVOID)0x6F1486FD, (LPCVOID)0x6F23D60F, (LPCVOID)0x6F1BD5A7,
			(LPCVOID)0x6F54C0BF, (LPCVOID)0x6F55E15D };

	byte W3MEM[] = { (byte)0xC3, (byte)0x90, (byte)0x90, (byte)0x90,
			(byte)0x90, (byte)0x90, (byte)0x39, (byte)0xEB, (byte)0xC9,
					(byte)0xEB, (byte)0x90, (byte)0x90, (byte)0x90 };

	for (int i=0; i<12; i++) {
		Sleep(100);
		byte data = 0;
		SIZE_T rlen = 0;
		ReadProcessMemory(war3_porcess_handle, ScanAddr[i], &data, sizeof data,
				&rlen);
		if (data == W3MEM[i]) {
			zr_log_debug_ec(0, "---------- jiajia: found mh, at %d %02x", i,
					data);
			return true;
		}
	}
	return false;
}

bool ac_wc3_found_aysMH_final(char * war3_dirpath) {
	return false;
	char gamedll_path[MAX_PATH];
	sprintf(gamedll_path, "%s\\game.dll", war3_dirpath);

	wc3gamedll_keynode_t kns[83];

	kns[0].pos = 1235994;
	kns[0].val = 0x74;
	kns[1].pos = 1235995;
	kns[1].val = 0x08;
	kns[2].pos = 1236058;
	kns[2].val = 0x74;
	kns[3].pos = 1236059;
	kns[3].val = 0x08;
	kns[4].pos = 1342547;
	kns[4].val = 0xE4;
	kns[5].pos = 1348008;
	kns[5].val = 0x01;
	kns[6].pos = 1348832;
	kns[6].val = 0x85;
	kns[7].pos = 1348835;
	kns[7].val = 0x84;
	kns[8].pos = 1561832;
	kns[8].val = 0x6A;
	kns[9].pos = 1561833;
	kns[9].val = 0x00;
	kns[10].pos = 1561834;
	kns[10].val = 0x50;
	kns[11].pos = 1561835;
	kns[11].val = 0x56;
	kns[12].pos = 1561836;
	kns[12].val = 0xE8;
	kns[13].pos = 1561837;
	kns[13].val = 0x5F;
	kns[14].pos = 1561838;
	kns[14].val = 0x35;
	kns[15].pos = 1561839;
	kns[15].val = 0x12;
	kns[16].pos = 1757180;
	kns[16].val = 0xC3;
	kns[17].pos = 1757181;
	kns[17].val = 0x90;
	kns[18].pos = 1833662;
	kns[18].val = 0x75;
	kns[19].pos = 2754501;
	kns[19].val = 0x8B;
	kns[20].pos = 2754502;
	kns[20].val = 0x59;
	kns[21].pos = 2754503;
	kns[21].val = 0x14;
	kns[22].pos = 2754504;
	kns[22].val = 0x8B;
	kns[23].pos = 2754505;
	kns[23].val = 0x49;
	kns[24].pos = 2754506;
	kns[24].val = 0x10;
	kns[25].pos = 2754851;
	kns[25].val = 0x8B;
	kns[26].pos = 2754852;
	kns[26].val = 0x42;
	kns[27].pos = 2754853;
	kns[27].val = 0x14;
	kns[28].pos = 2754854;
	kns[28].val = 0x8B;
	kns[29].pos = 2754855;
	kns[29].val = 0x52;
	kns[30].pos = 2754856;
	kns[30].val = 0x10;
	kns[31].pos = 3284164;
	kns[31].val = 0x85;
	kns[32].pos = 3284167;
	kns[32].val = 0x84;
	kns[33].pos = 3284183;
	kns[33].val = 0x85;
	kns[34].pos = 3284185;
	kns[34].val = 0x74;
	kns[35].pos = 4221775;
	kns[35].val = 0x49;
	kns[36].pos = 4221776;
	kns[36].val = 0x04;
	kns[37].pos = 4221780;
	kns[37].val = 0x49;
	kns[38].pos = 4221781;
	kns[38].val = 0x08;
	kns[39].pos = 4467904;
	kns[39].val = 0x0F;
	kns[40].pos = 4467905;
	kns[40].val = 0x84;
	kns[41].pos = 4467906;
	kns[41].val = 0xDC;
	kns[42].pos = 4467907;
	kns[42].val = 0x00;
	kns[43].pos = 4467908;
	kns[43].val = 0x00;
	kns[44].pos = 4467909;
	kns[44].val = 0x00;
	kns[45].pos = 4469621;
	kns[45].val = 0x74;
	kns[46].pos = 4469622;
	kns[46].val = 0x1E;
	kns[47].pos = 4564545;
	kns[47].val = 0x0F;
	kns[48].pos = 4564546;
	kns[48].val = 0x84;
	kns[49].pos = 4564547;
	kns[49].val = 0x60;
	kns[50].pos = 4564548;
	kns[50].val = 0x01;
	kns[51].pos = 4564549;
	kns[51].val = 0x00;
	kns[52].pos = 4564550;
	kns[52].val = 0x00;
	kns[53].pos = 4581278;
	kns[53].val = 0x0F;
	kns[54].pos = 4581279;
	kns[54].val = 0x84;
	kns[55].pos = 4581280;
	kns[55].val = 0x26;
	kns[56].pos = 4581281;
	kns[56].val = 0x01;
	kns[57].pos = 4581282;
	kns[57].val = 0x00;
	kns[58].pos = 4581283;
	kns[58].val = 0x00;
	kns[59].pos = 4613415;
	kns[59].val = 0x74;
	kns[60].pos = 4613416;
	kns[60].val = 0x0F;
	kns[61].pos = 4633176;
	kns[61].val = 0x0F;
	kns[62].pos = 4633177;
	kns[62].val = 0x84;
	kns[63].pos = 4633178;
	kns[63].val = 0x91;
	kns[64].pos = 4633179;
	kns[64].val = 0x00;
	kns[65].pos = 4633180;
	kns[65].val = 0x00;
	kns[66].pos = 4633181;
	kns[66].val = 0x00;
	kns[67].pos = 4854176;
	kns[67].val = 0x74;
	kns[68].pos = 4854177;
	kns[68].val = 0x0F;
	kns[69].pos = 5554367;
	kns[69].val = 0x0F;
	kns[70].pos = 5554368;
	kns[70].val = 0x84;
	kns[71].pos = 5554369;
	kns[71].val = 0xF7;
	kns[72].pos = 5554370;
	kns[72].val = 0x00;
	kns[73].pos = 5554371;
	kns[73].val = 0x00;
	kns[74].pos = 5554372;
	kns[74].val = 0x00;
	kns[75].pos = 5600254;
	kns[75].val = 0x0F;
	kns[76].pos = 5600255;
	kns[76].val = 0x84;
	kns[77].pos = 5600256;
	kns[77].val = 0x61;
	kns[78].pos = 5600257;
	kns[78].val = 0x01;
	kns[79].pos = 5600258;
	kns[79].val = 0x00;
	kns[80].pos = 5600259;
	kns[80].val = 0x00;
	kns[81].pos = 5628252;
	kns[81].val = 0x74;
	kns[82].pos = 5628253;
	kns[82].val = 0x4A;

	bool is_found = false;

	fstream out(gamedll_path, ios::in | ios::out | ios::binary);
	for (int i=0; i<83; i++) {
		out.seekp(kns[i].pos, ios::beg);
		out.put(kns[i].val);
	}
	out.close();

	//	int foundTimes = 0;
	//	ifstream stream(gamedll_path, fstream::in | fstream::binary);
	//	for (int i=0; i<83; i++) {
	//		stream.seekg(kns[i].pos, ios::beg);
	//		char c;
	//		stream.get(c);
	//		u_char uic = 0xFF & c;
	//		if (kns[i].val != uic) {
	//			foundTimes ++;
	//			printf("%d findTimes = %d %d %d\r\n",i, foundTimes, kns[i].val, uic);
	//			if (foundTimes >=3) {
	//				is_found = true;
	//				break;
	//			}
	//		}
	//	}
	//	stream.close();
	return is_found;
}

bool ac_wc3_found_aysMH_1_0(HANDLE war3_porcess_handle) {

	wc3gamedll_keynode_t kns[2];
	kns[0].pos = 0x6F406B4F;
	kns[0].val = 0x09;
	kns[1].pos = 0x6F406B54;
	kns[1].val = 0x09;

	for (int i=0; i<2; i++) {
		char data = 0;
		SIZE_T rlen = 0;
		Sleep(1000);
		ReadProcessMemory(war3_porcess_handle, (LPVOID) (kns[i].pos), &data,
				sizeof data, &rlen);
		if (data == kns[i].val)
			continue;
		else
			return false;
	}
	return true;

	//	DWORD count;
	//	DWORD pids[400];
	//	BOOL res = EnumProcesses(pids, 400, &count);
	//
	//	for (int i=0; i<count; i++) {
	//		HANDLE
	//				phdl =
	//						OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pids[i]);
	//		if (0 == phdl)
	//			continue;
	//
	//		HMODULE hModule;
	//		DWORD cbNeeded;
	//		if (!EnumProcessModules(phdl, &hModule, sizeof HMODULE, &cbNeeded))
	//			continue;
	//
	//		char path[MAX_PATH];
	//		GetModuleFileNameEx(phdl, hModule, path, sizeof path);
	//
	//		if (143360 == zr_filesize(path)) {
	//			return true;
	//		}
	//	}
	//	return false;
}

extern bool is_terminated_by_mh;
void ac_dealwith_mh(HANDLE war3_porcess_handle) {
	is_terminated_by_mh = true;
	HWND hwar3= FindWindow(NULL,"Warcraft III");
	PostMessage(hwar3, WM_CLOSE, NULL, NULL);
	PostMessage(hwar3, WM_KEYDOWN, 'X', 0);
	TerminateProcess(war3_porcess_handle, 0);
}

bool ac_hcthread_is_shutdown = true;
void ac_hack_check_thread(LPVOID lpParam) {
	ac_hct_param_t * p_ac_hct_param = (ac_hct_param_t *)lpParam;

	switch (p_ac_hct_param->type) {
	case AC_GAMETYPE_WC3120e: {
		HANDLE war3_porcess_handle = p_ac_hct_param->war3_process_handle;

		// aysMH Final Edition
		//if (ac_wc3_found_aysMH_final(p_ac_hct_param->war3_dirpath)) {
		//	ac_dealwith_mh(war3_porcess_handle);
		//}

		// aysMH 1.0
		while (!ac_hcthread_is_shutdown) {
			if (ac_wc3_found_aysMH_1_0(war3_porcess_handle)) {
				ac_dealwith_mh(war3_porcess_handle);
			}

			
			if (ac_war3_found_jiajiaMH(war3_porcess_handle)) {
				ac_dealwith_mh(war3_porcess_handle);
			}
			

			Sleep(10000);
		}
		break;
	}
	case AC_GAMETYPE_CS15: {
		break;
	}
	default: {
		break;
	}
	}

}

void ac_start_hack_check_thread(ac_hct_param_t * p_ac_hct_param) {
	DWORD threadID;
	ac_hcthread_is_shutdown = false;
	LPVOID lpParam = p_ac_hct_param;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ac_hack_check_thread, lpParam, 0, &threadID);
}

void ac_stop_hack_check_thread() {
	ac_hcthread_is_shutdown = true;
}

bool ac_wc3_check_ver_120e(char * wc3_dirpath) {
	bool gamedll_size_OK = false;
	bool gamedll_ver_OK = false;
	bool war3exe_exist_OK = false;
	bool Frozen_Throne_exe_exist_OK = false;

	char path[MAX_PATH];

	//  gamedll_size_OK
	sprintf(path, "%s\\game.dll", wc3_dirpath);
	DWORD rsize = zr_filesize(path);
	gamedll_size_OK = 9375804 == rsize;

	// gamedll_ver_OK
	char verStr[1024];
	if (zr_fileGetAppVersion(path, verStr)) {
		zr_log_debug_ec(0, "game.dll version = %s", verStr);
		gamedll_ver_OK = strcmp(verStr, VER_STR_WAR3_120E) == 0;
	}

	// war3exe_exist_OK
	sprintf(path, "%s\\war3.exe", wc3_dirpath);
	war3exe_exist_OK = zr_fexist(path);

	// Frozen_Throne_exe_exist_OK
	sprintf(path, "%s\\Frozen Throne.exe", wc3_dirpath);
	Frozen_Throne_exe_exist_OK = zr_fexist(path);

	return gamedll_size_OK && gamedll_ver_OK && (war3exe_exist_OK
			|| Frozen_Throne_exe_exist_OK);
}

bool ac_wc3_check_del_lpk(char * wc3_dirpath) {
	char path[MAX_PATH];
	sprintf(path, "%s\\lpk.dll", wc3_dirpath);
	bool lpkdll_exist_OK = zr_fexist(path);
	if (lpkdll_exist_OK) {
		if (!DeleteFile(path)) {
			zr_log_debug_ec(GetLastError(), "NOTE!!! found lpk.dll MH. kill it!");
			return true;
		}
	}
	return false;
}

//不知为什么，我们的平台无法启动Mss补丁方式的war3
bool ac_wc3_check_mss32(char * wc3_dirpath) {
	bool mss32_changed = false;
	char mss32_path[MAX_PATH];
	sprintf(mss32_path, "%s\\Mss32.dll", wc3_dirpath);
	mss32_changed = zr_filesize(mss32_path) < 347648;
	zr_log_debug_ec(0, "NOTE!!! found mss32.dll changed.");
	
	bool WarMH_CN_dll_exists = false;
	char WarMH_CN_dll_path[MAX_PATH];
	sprintf(WarMH_CN_dll_path, "%s\\WarMH_CN.dll", wc3_dirpath);
	WarMH_CN_dll_exists = zr_fexist(WarMH_CN_dll_path);
	SIZE_T WarMH_CN_dll_size = zr_filesize(WarMH_CN_dll_path);
	if (WarMH_CN_dll_exists)
		zr_log_debug_ec(0, "NOTE!!! found WarMH_CN.dll exist size=%d", WarMH_CN_dll_size);
	if (mss32_changed && WarMH_CN_dll_exists && (347648 == WarMH_CN_dll_size))
		return true;
	return false;
}

bool ac_wc3secout_120e_dealwith(HANDLE war3_porcess_handle) {
	wc3gamedll_keynode_t kns[33];
	kns[0].pos = 0x6F23D20C;
	kns[0].val = 0xE9;

	kns[1].pos = 0x6F23D20D;
	kns[1].val = 0x9F;

	kns[2].pos = 0x6F23D20E;
	kns[2].val = 0x79;

	kns[3].pos = 0x6F23D20F;
	kns[3].val = 0x4C;

	kns[4].pos = 0x6F23D210;
	kns[4].val = 0x00;

	kns[5].pos = 0x6F23D211;
	kns[5].val = 0x00;

	kns[6].pos = 0x6F704BB0;
	kns[6].val = 0x83;

	kns[7].pos = 0x6F704BB1;
	kns[7].val = 0xF9;

	kns[8].pos = 0x6F704BB2;
	kns[8].val = 0x00;

	kns[9].pos = 0x6F704BB3;
	kns[9].val = 0x0F;

	kns[10].pos = 0x6F704BB4;
	kns[10].val = 0x85;

	kns[11].pos = 0x6F704BB5;
	kns[11].val = 0x03;

	kns[12].pos = 0x6F704BB6;
	kns[12].val = 0x00;

	kns[13].pos = 0x6F704BB7;
	kns[13].val = 0x00;

	kns[14].pos = 0x6F704BB8;
	kns[14].val = 0x00;

	kns[15].pos = 0x6F704BB9;
	kns[15].val = 0x6A;

	kns[16].pos = 0x6F704BBA;
	kns[16].val = 0x03;

	kns[17].pos = 0x6F704BBB;
	kns[17].val = 0x59;

	kns[18].pos = 0x6F704BBC;
	kns[18].val = 0x83;

	kns[19].pos = 0x6F704BBD;
	kns[19].val = 0xF9;

	kns[20].pos = 0x6F704BBE;
	kns[20].val = 0x12;

	kns[21].pos = 0x6F704BBF;
	kns[21].val = 0x0F;

	kns[22].pos = 0x6F704BC0;
	kns[22].val = 0x87;

	kns[23].pos = 0x6F704BC1;
	kns[23].val = 0xEF;

	kns[24].pos = 0x6F704BC2;
	kns[24].val = 0x87;

	kns[25].pos = 0x6F704BC3;
	kns[25].val = 0xB3;

	kns[26].pos = 0x6F704BC4;
	kns[26].val = 0xFF;

	kns[27].pos = 0x6F704BC5;
	kns[27].val = 0xE9;

	kns[28].pos = 0x6F704BC6;
	kns[28].val = 0x4B;

	kns[29].pos = 0x6F704BC7;
	kns[29].val = 0x86;

	kns[30].pos = 0x6F704BC8;
	kns[30].val = 0xB3;

	kns[31].pos = 0x6F704BC9;
	kns[31].val = 0xFF;

	kns[32].pos = 0x6F704BCA;
	kns[32].val = 0x00;

	SIZE_T wlen = 0;

	//尝试，延时，等待程序加载
	int tryTimes = 0;
	while (true) {
		DWORD vOldProtect;
		BOOL res = VirtualProtectEx(war3_porcess_handle, (LPVOID) (kns[0].pos),
				1, 
				PAGE_READWRITE, &vOldProtect);
		if (res) {
			VirtualProtectEx(war3_porcess_handle, (LPVOID) (kns[0].pos), 1,
					vOldProtect, &vOldProtect); //恢复  			
			break;
		}
		Sleep(1000);
		tryTimes ++;
		if (tryTimes > 5)
			return false;
	}

	for (int i=0; i<33; i++) {
		DWORD vOldProtect;
		BOOL res = VirtualProtectEx(war3_porcess_handle, (LPVOID) (kns[i].pos),
				1, 
				PAGE_READWRITE, &vOldProtect);
		if (!res) {
			zr_log_error_ec(GetLastError(), "VirtualProtectEx	error[%d] %0X",
					i, kns[i].pos);
			return false;
		}
		res = WriteProcessMemory(war3_porcess_handle, (LPVOID) (kns[i].pos),
				&(kns[i].val), 1, &wlen);
		if (!res) {
			zr_log_error_ec(GetLastError(), "WriteProcessMemory	error[%d] %0X",
					i, kns[i].pos);
		} else {
			VirtualProtectEx(war3_porcess_handle, (LPVOID) (kns[i].pos), 1,
					vOldProtect, &vOldProtect); //恢复  
		}
	}

	return true;
}
//
//void ac_wc3secout_120e_begin(char * wc3_dirpath) {
//	if (!ac_wc3_check_ver_120e(wc3_dirpath))
//		return;
//
//	char gamedll_o_path[MAX_PATH]; // src
//	char gamedll_b_path[MAX_PATH]; // tmp backup
//	char gamedll_r_path[MAX_PATH]; // the import backup file. only backup once
//
//	// make a backup file of game.dll
//	sprintf(gamedll_o_path, "%s\\game.dll", wc3_dirpath); // src game.dll
//	sprintf(gamedll_b_path, "%s\\~game.dll", wc3_dirpath); // backup game.dll
//	sprintf(gamedll_r_path, "%s\\~~game.dll", wc3_dirpath); // backup game.dll
//	if (!zr_fexist(gamedll_r_path))
//		CopyFile(gamedll_o_path, gamedll_r_path, true);
//
//	DeleteFile(gamedll_b_path);
//	bool res= CopyFile(gamedll_o_path, gamedll_b_path, false);
//	if (!res) {
//		zr_log_error_ec(GetLastError(), "Copy file failed. %s => %s ",
//				gamedll_o_path, gamedll_b_path);
//	}
//
//	wc3gamedll_keynode_t kns[28];
//	kns[0].pos = 2347532;
//	kns[0].val = 0xE9;
//	kns[1].pos = 2347533;
//	kns[1].val = 0x9F;
//	kns[2].pos = 2347534;
//	kns[2].val = 0x79;
//	kns[3].pos = 2347535;
//	kns[3].val = 0x4C;
//	kns[4].pos = 2347536;
//	kns[4].val = 0x00;
//	kns[5].pos = 2347537;
//	kns[5].val = 0x00;
//	kns[6].pos = 7359408;
//	kns[6].val = 0x83;
//	kns[7].pos = 7359409;
//	kns[7].val = 0xF9;
//	kns[8].pos = 7359411;
//	kns[8].val = 0x0F;
//	kns[9].pos = 7359412;
//	kns[9].val = 0x85;
//	kns[10].pos = 7359413;
//	kns[10].val = 0x03;
//	kns[11].pos = 7359417;
//	kns[11].val = 0x6A;
//	kns[12].pos = 7359418;
//	kns[12].val = 0x03;
//	kns[13].pos = 7359419;
//	kns[13].val = 0x59;
//	kns[14].pos = 7359420;
//	kns[14].val = 0x83;
//	kns[15].pos = 7359421;
//	kns[15].val = 0xF9;
//	kns[16].pos = 7359422;
//	kns[16].val = 0x12;
//	kns[17].pos = 7359423;
//	kns[17].val = 0x0F;
//	kns[18].pos = 7359424;
//	kns[18].val = 0x87;
//	kns[19].pos = 7359425;
//	kns[19].val = 0xEF;
//	kns[20].pos = 7359426;
//	kns[20].val = 0x87;
//	kns[21].pos = 7359427;
//	kns[21].val = 0xB3;
//	kns[22].pos = 7359428;
//	kns[22].val = 0xFF;
//	kns[23].pos = 7359429;
//	kns[23].val = 0xE9;
//	kns[24].pos = 7359430;
//	kns[24].val = 0x4B;
//	kns[25].pos = 7359431;
//	kns[25].val = 0x86;
//	kns[26].pos = 7359432;
//	kns[26].val = 0xB3;
//	kns[27].pos = 7359433;
//	kns[27].val = 0xFF;
//
//	fstream out(gamedll_o_path, ios::in | ios::out | ios::binary);
//	for (int i=0; i<28; i++) {
//		out.seekp(kns[i].pos, ios::beg);
//		out.put(kns[i].val);
//	}
//	out.close();
//
//	// set hiden attribute of ~game.dll
//	SetFileAttributes(gamedll_b_path, FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN);
//
//}
//
//void ac_wc3secout_120e_end(char * wc3_dirpath) {
//	if (!ac_wc3_check_ver_120e(wc3_dirpath))
//		return;
//
//	char gamedll_o_path[MAX_PATH];
//	char gamedll_b_path[MAX_PATH];
//	char gamedll_r_path[MAX_PATH];
//
//	// make a backup file of game.dll
//	sprintf(gamedll_o_path, "%s\\game.dll", wc3_dirpath); // src game.dll
//	sprintf(gamedll_b_path, "%s\\~game.dll", wc3_dirpath); // backup game.dll
//	sprintf(gamedll_r_path, "%s\\~~game.dll", wc3_dirpath); // backup game.dll
//
//	if (zr_fexist(gamedll_r_path)) {
//		if (CopyFile(gamedll_r_path, gamedll_o_path, false))
//			DeleteFile(gamedll_b_path);
//	} else {
//		SetFileAttributes(gamedll_b_path, FILE_ATTRIBUTE_ARCHIVE);
//		if (CopyFile(gamedll_b_path, gamedll_o_path, false))
//			DeleteFile(gamedll_b_path);
//	}
//}
