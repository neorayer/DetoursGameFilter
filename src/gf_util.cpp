#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "gf_props.h"
#include "gf_util.h"

//获取录像保存目录
void GetGfReplaysDirPath(char *pathBuf) {
	zr_fileVarStr_get("replaysDirPath", pathBuf);

}


void saveWar3Replay() {
	//if (!isWar3)
	//	return;

	char game_dir[MAX_PATH];
	zr_fileVarStr_get("gameDir", game_dir);

	// init Create Replay Directory 
	char gfReplaysDirPath[MAX_PATH];
	GetGfReplaysDirPath(gfReplaysDirPath);
	CreateDirectory(gfReplaysDirPath, NULL);

	char lastReplayPath[MAX_PATH];
	sprintf(lastReplayPath, "%s\\replay\\LastReplay.w3g", game_dir);
	char newReplayPath[MAX_PATH];
	char timeStr[MAX_PATH];
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	sprintf(timeStr, "KO_%d-%02d-%02d_%02d-%02d-%02d", lt.wYear, lt.wMonth,
			lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);

	char mvLastReplayPath[MAX_PATH];
	sprintf(mvLastReplayPath, "%s\\replay\\MyLastReplay.w3g", game_dir);

	sprintf(newReplayPath, "%s\\%s.w3g", gfReplaysDirPath, timeStr);
	int repeatTimes = 0;
	while (true) {
		if (zr_fexist(lastReplayPath)) {
			BOOL res= CopyFile(lastReplayPath, newReplayPath, true);
			zr_log_debug_ec(res, "save replay to %s", newReplayPath);
			res = CopyFile(lastReplayPath, mvLastReplayPath, false);
			zr_log_debug_ec(res, "copy MyLastReplay");
			res = DeleteFile(lastReplayPath);
			zr_log_debug_ec(res, "del LastReplay");
			break;
		}
		Sleep(5000);
		repeatTimes ++;
		if (repeatTimes >12)
			break;
	}
}