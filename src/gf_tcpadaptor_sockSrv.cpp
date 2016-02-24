// TcpPipeAdaptor.cpp : Defines the entry point for the console application.
//

#include <winsock2.h>
#include <String.h>
#include <stdio.h>
#include <stdlib.h>

#include <zr_logger.h>
#include <zr_file.h>
#include <zr_netutils.h>
#include <zr_sock.h>
#include <zr_sockVarSrv.h>

#include "gf_props.h"
#include "gf_head.h"

typedef struct SlotRecord {
	byte playerId;
	byte mapDown;
	byte slotStatus;
	byte computerPlayerFlag;
	byte teamNumber;
	byte color;
	byte playerRaceFlags;
	byte computerAiStrength;
	byte playerHandicap;
} t_slot_record;

#define GAME_RES_UNKNOWN  0
#define GAME_RES_HOST_WIN  1
#define GAME_RES_HOST_LOST  2
#define GAME_RES_EXCEPTION  32

//int gameResult= GAME_RES_UNKNOWN;
int beginWinnerScanRes = -1;

#define GAME_RES_REASON_UNKNOWN 0
#define GAME_RES_REASON_FULL  1
#define GAME_RES_REASON_LEAVE  2
//int gameResultReason= GAME_RES_REASON_FULL;

//bool isGameResultSure = false;

bool isWar3 = false;

SOCKET sock;
SOCKET destSock;
gf_props_t props;

bool isShutdown = false;
char gamerUsername[32];
byte gamerTeamNum = -1;
byte gamerPlayerId = -1;
int slotCount = -1;
t_slot_record srs[20];
bool isHostCancel = false;

int hTeamCount = 0;
int oTeamCount = 0;

//byte hostTeamNum;

char hostTeamS[512];
char otherTeamS[512];

char fightDirPath[MAX_PATH];
char gamerSrPath[MAX_PATH];
char gamerIdPath[MAX_PATH];
BOOL isFightBegin = false;
int hostTeamLess = 0;
int otherTeamLess = 0;
int lessGamerCount;

int beginTime = 0;
int readyTime = 0;
int endTime = 0;

int teamCount = 0;

int gameSpendSeconds = 0;

#define LEAVE_REASON_UNKNOWN 0
#define LEAVE_REASON_LEAVE 1
#define LEAVE_REASON_WIN 2
#define LEAVE_REASON_LOST 3
//int gamerLeaveReason= LEAVE_REASON_UNKNOWN;

zr_sockVarClt * varClt;

void GetGfReplaysDirPath(char *pathBuf) {
	GetModuleFileName(NULL,
			pathBuf, MAX_PATH);
	char *pos = strrchr(pathBuf, '\\');
	*pos = '\0';
	pos = strrchr(pathBuf, '\\');
	*pos = '\0';

	strcat(pathBuf, "\\w3gReplays");
}

void setMapChecksum(int mapCheckSum) {
	varClt->setInt("MapChecksum", mapCheckSum);
}
int getMapChecksum() {
	return varClt->getInt("MapChecksum");
}

////////////////////////////////////////////////////////////
void saveWar3Replay() {
	if (!isWar3)
		return;

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
	sprintf(timeStr, "%d-%02d-%02d_%02d-%02d-%02d", lt.wYear, lt.wMonth,
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

///////////////////////////////////
void setGameResultVars(bool isGameResultSure, int gameResult,
		int gameResultReason, char * desc) {
	zr_log_debug_ec(0, "setGameResultVars(%d, %d, %d): desc=%s",
			isGameResultSure, gameResult, gameResultReason, desc);
	varClt->setInt("gameResult", gameResult);
	zr_fileVarBool_set("isGameResultSure", isGameResultSure);
	varClt->setInt("gameResultReason", gameResultReason);
}

void setGamerLeaveReason(char *username, int leaveReason) {
	char buf[128];
	sprintf(buf, "LeaveReason_%s", username);
	varClt->setInt(buf, leaveReason);
	zr_log_debug_ec(0, "setGamerLeaveReason %s=%d", username, leaveReason);
}

int getGamerLeaveReason(char *username) {
	char buf[128];
	sprintf(buf, "LeaveReason_%s", username);
	return varClt->getInt(buf);
}

void setTeamAllLeaveTime(int teamNum) {
	char varName[128];
	sprintf(varName, "team_%d", teamNum);
	varClt->setInt(varName, time(NULL));
}

int getTeamAllLeaveTime(int teamNum) {
	char varName[128];
	sprintf(varName, "team_%d", teamNum);
	return varClt->getInt(varName);
}

///////////////////////////////////
#define ERR_TEAM_NUM 100
byte getHostTeamNum() {
	for (int i=0; i<slotCount; i++) {
		if (srs[i].playerId == 1)
			return srs[i].teamNumber;
	}
	return ERR_TEAM_NUM;
}

byte getOtherTeamNum() {
	int hostTeamNum = getHostTeamNum();
	for (int i=0; i<slotCount; i++) {
		if (srs[i].playerId <=1)
			continue;
		if (srs[i].teamNumber == hostTeamNum)
			continue;
		return srs[i].teamNumber;
	}
	return ERR_TEAM_NUM;
}

byte getTeamCount() {
	byte teamCs[12];
	for (int i=0; i<12; i++)
		teamCs[i] = 0;
	for (i=0; i<slotCount; i++) {
		if (srs[i].playerId <= 0)
			continue;
		teamCs[srs[i].teamNumber] ++;
	}
	byte c =0;
	for (i=0; i<12; i++)
		if (teamCs[i] > 0)
			c++;
	return c;
}

byte getHostTeamGamersCount() {
	int hostTeamNum = getHostTeamNum();
	byte c = 0;
	for (int i=0; i<slotCount; i++) {
		if (srs[i].playerId <= 0)
			continue;
		if (srs[i].teamNumber == hostTeamNum)
			c++;
	}
	return c;
}

byte getOtherTeamGamersCount() {
	int hostTeamNum = getHostTeamNum();
	byte c = 0;
	for (int i=0; i<slotCount; i++) {
		if (srs[i].playerId <= 0)
			continue;
		if (srs[i].teamNumber != hostTeamNum)
			c++;
	}
	return c;

}

void setUsernameByPlayerId(byte id, char * name) {
	char unPath[MAX_PATH];
	sprintf(unPath, "%s\\%d.un", fightDirPath, id);
	zr_str2file(name, unPath);
	zr_log_debug_ec(0, "user:%s  playerId=%d", name, id);
}

void getUsernameByPlayerId(byte id, char * buf) {
	char unPath[MAX_PATH];
	sprintf(unPath, "%s\\%d.un", fightDirPath, id);
	zr_file2strbuf(unPath, buf);
}

int waitForWar3WinnerScan() {
	zr_log_debug("waitForWar3WinnerScan");
	char buf[128];
	sprintf(buf, "0");
	zr_sock_startup();
	ZrSocket * zrSocket = new ZrSocket();
	if (!zrSocket->connect("127.0.0.1", 8497)) {
		zr_log_error_ec(WSAGetLastError(), "connect 127.0.0.1:8497 Failed");
		return 0;
	}
	int rLen = zrSocket->recv(buf, 128);
	if (rLen < 0) {
		zr_log_error_ec(WSAGetLastError(), "access Socket recv Error.");
		return 0;
	}
	zr_log_debug_ec(0, "Get War3MemScanRes = %s", buf);
	int res = atoi(buf);
	zrSocket->close();
	zr_sock_cleanup();
	delete zrSocket;

	return res;
}

bool isWinCheck() {
	int mapChecksum = getMapChecksum();
	zr_log_debug_ec(0, "isWinCheck mapChecksum=%d", mapChecksum);
	switch (mapChecksum) {
	case MAP_CHKSUM_Z3_39D:
	case MAP_CHKSUM_Z3_42E:

	case MAP_CHKSUM_ORC3C_169V:
	case MAP_CHESUM_ORC3C_184_FIX2:
	case MAP_CHESUM_ORC3C_185_L7:
	case MAP_CHESUM_ORC3C_185_B1:

	case MAP_CHESUM_DOTA_651_CN:

	case MAP_CHKSUM_LST_1:
	case MAP_CHKSUM_LST_2:
	case MAP_CHKSUM_ECHO_ISLES_1:
	case MAP_CHKSUM_TERENAS_STAND_1:
	case MAP_CHKSUM_ISLANDS_1:
	case MAP_CHKSUM_TURTLE_ROCK_1:
	case MAP_CHESUM_TWISTED_MEADOWS_1:
	case MAP_CHESUM_GNOLL_WOOD_1: {
		break;
	}
	default: {
		return false;
		break;
	}
	}

	// 判定队伍数量
	if (teamCount != 2) {
		zr_log_debug_ec(0, "NOT winner Check becouse teamCount=%d", teamCount);
		return false;
	}

	//人数判定
	zr_log_debug_ec(0, "Mode %d vs %d", hTeamCount, oTeamCount);
	if (hTeamCount != oTeamCount) {
		//zr_log_debug_ec(0, "NOT winner Check becouse user count NOT balance.");
		// return false;
	}
	return true;
}

void buildGameResultStr(char * resStr) {
	resStr[0] = '\0';
	bool isGameResultSure = zr_fileVarBool_get("isGameResultSure");
	int gameResult = varClt->getInt("gameResult");
	int gameResultReason = varClt->getInt("gameResultReason");
	int hostTeamNum = getHostTeamNum();
	int otherTeamNum = getOtherTeamNum();
	int mapChecksum = getMapChecksum();
	if (isGameResultSure) {
		zr_log_debug("Game Result SURE!");
		// NOTE: orc3c中，对方全退，我方没退，会认为“完胜”，而不是“对方认输”
		switch (gameResult) {
		case GAME_RES_HOST_WIN:
			sprintf(resStr, "1||%s||%s||%d||%d||%d||%d", hostTeamS, otherTeamS,
					gameResultReason, gameSpendSeconds, hostTeamNum,
					mapChecksum);
			break;
		case GAME_RES_HOST_LOST:
			sprintf(resStr, "1||%s||%s||%d||%d||%d||%d", otherTeamS, hostTeamS,
					gameResultReason, gameSpendSeconds, otherTeamNum,
					mapChecksum);
			break;
		default:
			sprintf(resStr, "0||%s||%s||%d||%d||%d||%d", hostTeamS, otherTeamS,
					gameResultReason, gameSpendSeconds, otherTeamNum,
					mapChecksum);
			break;
		}
		return;
	}
}

void reportGameRes() {
	char resStr[1024];
	buildGameResultStr(resStr);

	char url[1024];
	sprintf(
			url,
			"http://60.191.220.102/GameHall/pg/if/gh/if_game_res.jsp?username=%s&res=%s",
			props.username, resStr);
	zr_log_debug_ec(0, "report res %s", url);
	zr_sock_startup();
	zr_http_request(url, NULL);
	zr_sock_cleanup();
}

int getUsersCount() {
	int c =0;
	for (int i=0; i<slotCount; i++) {
		if (srs[i].playerId > 0)
			c++;
	}
	return c;
}

int getLessUserCount() {
	int c =getUsersCount();
	for (int i=0; i<slotCount; i++) {
		// skip computer player
		int playerId = srs[i].playerId;
		if (playerId <= 0)
			continue;

		if (playerId == 1)
			continue;
		char path[MAX_PATH];
		sprintf(path, "%s\\%d.ext", fightDirPath, playerId);
		if (!zr_fexist(path))
			c--;
	}
	return c;
}

void saveSlotRecords() {
	sprintf(gamerSrPath, "%s\\%s.slotRecords", fightDirPath, gamerUsername);
	zr_mem2file(srs, slotCount *(sizeof t_slot_record), gamerSrPath);
	zr_log_debug_ec(0, "UserCount = %d", getUsersCount());
	//	//TODO:
	//	t_slot_record new_srs[20];
	//	zr_file2memBuf(path, new_srs, slotCount *(sizeof t_slot_record));
	//
	//	zr_log_debug("************************************");
	//	{
	//		for (int i=0; i<slotCount; i++) {
	//			zr_log_debug_ec(0, "%d pid=%d tn=%d", i, new_srs[i].playerId,
	//					new_srs[i].teamNumber);
	//		}
	//	}
	//
}

void inline filterSinglePkgHostIn(char * buf, char offSet /*The pos of 0xf7*/,
		int onePkgLen, int len) {
	byte actTag = (byte)buf[offSet + 1];
	// zr_log_debug_ec(0, "filter actTag=%d", actTag);
	switch (actTag) {
	case 0x21: { // gamerExist
		byte reasonTag = (byte)buf[offSet + 4];
		if (0x07 == reasonTag) {
			//标记用户离开原因
			setGamerLeaveReason(gamerUsername, LEAVE_REASON_LEAVE);
		} else if (0x09 == reasonTag) {
			if (gamerTeamNum == hostTeamNum)
				setGameResultVars(true, GAME_RES_HOST_WIN, 
				GAME_RES_REASON_FULL, "Host get 0x09, from HostTeam gamer");
			else {
				setGameResultVars(true, GAME_RES_HOST_LOST, 
				GAME_RES_REASON_FULL, "Host get 0x09, from OtherTeam gamer");
				setGamerLeaveReason(gamerUsername, LEAVE_REASON_WIN);
			}
		}
		zr_log_debug_ec(0, "gamer  %s  send leave tag=%d", gamerUsername,
				reasonTag);
		break;
	}
	default:
		break;
	}
}
// server to client
void pipeThread1(LPVOID lpParam) {
	char buf[4096];
	isShutdown = false;
	while (true) {
		int rLen = recv(sock, buf, 4096, 0);
		if (rLen<=0)
			break;
		send(destSock, buf, rLen, 0);

		int off = 0;
		for (;;) {
			if ((byte)buf[off] != 0xf7) {
				zr_log_debug_ec(0, "%d != 0xf7", (byte)buf[off]);
				break;
			}
			int len = (byte)buf[off + 2] + (byte)buf[off + 3] * 256;
			if (len == 0) {
				zr_log_debug_ec(0, "pkgLen=0, STRANGE!!");
				break;
			}
			filterSinglePkgHostIn(buf, off, len, rLen);
			//zr_log_debug_ec(0, "off = %d; len = %d; rLen=%d", off, len, rLen);
			if (off + len >= rLen)
				break;
			off += len;
			// zr_log_debug_ec(0, "continue off=%d", off);
		}
	}

	isShutdown = true;
}

void inline filterSinglePkgHostOut(char * buf, char offSet /*The pos of 0xf7*/,
		int onePkgLen, int len) {
	byte actTag = (byte)buf[offSet + 1];
	// zr_log_debug_ec(0, "filter actTag=%d", actTag);
	switch (actTag) {
	case 0x04: {
		int slotCount = buf[offSet + 6];
		int off = 6 + slotCount * 9 + 7;
		gamerPlayerId = buf[off];
		if (gamerPlayerId == 0) {
			zr_log_warn_ec(0, "get [0] pid for  %s, so Set to 2", gamerUsername);
			gamerPlayerId = 2;
		}
		//保存用户ID对应文件
		setUsernameByPlayerId(gamerPlayerId, gamerUsername);

		break;
	}
	case 0x08: {
		// 正式战役开始时间
		readyTime = 0 == readyTime ? time(NULL) : readyTime;
		//		zr_log_debug(" ---=== Fight Fight Real Start (All Gamer Ready) ===---");
		break;
	}
	case 0x0a: {
		zr_log_debug(" ---=== Fight Begin (Init)===---");

		//开始时间
		beginTime = time(NULL);

		//初始化游戏结果参数
		setGameResultVars(false, GAME_RES_UNKNOWN, GAME_RES_REASON_UNKNOWN,
				"Init Fight Begin");

		zr_fileVarBool_set("IsGameOver", false);
		setGamerLeaveReason(gamerUsername, LEAVE_REASON_UNKNOWN);

		isFightBegin = true;
		saveSlotRecords();

		//保存本用户存在标记文件
		sprintf(gamerIdPath, "%s\\%d.ext", fightDirPath, gamerPlayerId);
		zr_str2file(gamerUsername, gamerIdPath);

		hostTeamNum = getHostTeamNum();
		zr_log_debug_ec(0, "HostTeamNum = %d", hostTeamNum);

		//记录游戏开始时, 内存扫描
		int mapChecksum = getMapChecksum();
		switch (mapChecksum) {
		case MAP_CHKSUM_Z3_39D:
		case MAP_CHKSUM_Z3_42E:
		case MAP_CHESUM_DOTA_651_CN: {
			beginWinnerScanRes = waitForWar3WinnerScan();
			zr_log_debug_ec(0, "beginWinnerScanRes=%d", beginWinnerScanRes);
			break;
		}
		default:
			break;
		}

		// 队伍数量
		teamCount = getTeamCount();

		// 对战双方人数
		hTeamCount = getHostTeamGamersCount();
		oTeamCount = getOtherTeamGamersCount();

		// 构造对战双方人员字串
		hostTeamS[0] = '\0';
		strcat(hostTeamS, props.username);
		strcat(hostTeamS, ":");
		otherTeamS[0] = '\0';
		for (int i=0; i<slotCount; i++) {
			byte playerId = srs[i].playerId;
			if (playerId <= 1) // skip computer && host
				continue;
			char username[128];
			getUsernameByPlayerId(playerId, username);
			if (srs[i].teamNumber == hostTeamNum) {
				strcat(hostTeamS, username);
				strcat(hostTeamS, ":");
			} else {
				strcat(otherTeamS, username);
				strcat(otherTeamS, ":");
			}
		}
		zr_log_debug_ec(0, "hostTeamS=%s, otherTeamS=%s", hostTeamS, otherTeamS);

		// set gameTeamNum
		for (i=0; i<slotCount; i++) {
			if (srs[i].playerId <=0) // skip computer player
				continue;
			if (srs[i].playerId == gamerPlayerId)
				gamerTeamNum = srs[i].teamNumber;
		}

		// Init TeamAllLeave time
		int hostTeamNum = getHostTeamNum();
		int otherTeamNum = getOtherTeamNum();
		setTeamAllLeaveTime(hostTeamNum);
		setTeamAllLeaveTime(otherTeamNum);

		// Get HostTeamCountLess and OtherTeamCountLess
		hostTeamNum = getHostTeamNum();
		hostTeamLess = 0;
		otherTeamLess = 0;
		for (i=0; i<slotCount; i++) {
			if (srs[i].playerId <=0) // skip computer player
				continue;
			if (srs[i].teamNumber == hostTeamNum)
				hostTeamLess ++;
			else
				otherTeamLess ++;
		}
		varClt->setInt("hostTeamLess", hostTeamLess);
		varClt->setInt("otherTeamLess", otherTeamLess);
		zr_log_debug_ec(0, "hostTeamLess=%d, otherTeamLess=%d", hostTeamLess,
				otherTeamLess);

		zr_fileVarBool_set("war3WinnerScan", false);
		zr_log_debug(" ---=== Fight Init over ===---");
		break;
	}
	case 0x14:
	case 0x1B: {
		// zr_log_debug_ec(0, "a fight End!! %d", (byte)buf[1]);
		break;
	}
	case 0x07: { // Finish Game
		char winTag = buf[offSet + 5];
		byte playerId = (byte)buf[offSet +4];
		if (0x07 == winTag) { // Host Cancel
			if (playerId == 1) {
				isHostCancel = true;
				zr_log_debug("set HostCancel true");
			}
		} else if (0x08 == winTag) { // Host Win
			int mapChecksum = getMapChecksum();
			switch (mapChecksum) {
			case MAP_CHKSUM_ORC3C_169V:
			case MAP_CHESUM_ORC3C_184_FIX2:
			case MAP_CHESUM_ORC3C_185_L7:
			case MAP_CHESUM_ORC3C_185_B1:

			case MAP_CHKSUM_LST_1:
			case MAP_CHKSUM_LST_2:
			case MAP_CHKSUM_ECHO_ISLES_1:
			case MAP_CHKSUM_TERENAS_STAND_1:
			case MAP_CHKSUM_ISLANDS_1:
			case MAP_CHKSUM_TURTLE_ROCK_1:
			case MAP_CHESUM_TWISTED_MEADOWS_1:
			case MAP_CHESUM_GNOLL_WOOD_1:
				zr_log_debug("I am sure the game result, from good map");
				setGameResultVars(true, GAME_RES_HOST_LOST, 
				GAME_RES_REASON_FULL, "Host send 0x08 && match MapChecksum");
				break;
			default:
				break;
			}
		} else if (0x09 == winTag) { // Host Lost
			setGameResultVars(true, GAME_RES_HOST_WIN, GAME_RES_REASON_FULL,
					"Host send 0x09");
		}
		break;
	}
	case 0x09: {
		slotCount = buf[offSet + 6];
		for (int i=0; i<slotCount; i++) {
			memcpy(&(srs[i]), (void *)(buf + offSet + 7 + i * 9),
					sizeof t_slot_record);
			//zr_log_debug_ec(0, "pid=%d tn=%d", srs[i].playerId, srs[i].teamNumber);
		}
		break;
	}
	case 0x3d: {
		int mapChecksumPos = offSet + onePkgLen - 4;
		byte b0 = (byte)buf[mapChecksumPos];
		byte b1 = (byte)buf[mapChecksumPos + 1];
		byte b2 = (byte)buf[mapChecksumPos + 2];
		byte b3 = (byte)buf[mapChecksumPos + 3];
		int mapChecksum = b3 * 256 * 256 * 256 + b2*256*256 + b1*256 + b0;
		zr_log_debug_ec(0, "get MapChecksum=%d", mapChecksum);
		setMapChecksum(mapChecksum);
	}
	default:
		break;
	}
}

// client to server
void pipeThread2(LPVOID lpParam) {
	char buf[4096];
	isShutdown = false;
	while (true) {
		int rLen = recv(destSock, buf, 4096, 0);
		if (rLen<=0)
			break;
		send(sock, buf, rLen, 0);

		int off = 0;
		for (;;) {
			if ((byte)buf[off] != 0xf7) {
				zr_log_debug_ec(0, "%d != 0xf7", (byte)buf[off]);
				break;
			}
			int len = (byte)buf[off + 2] + (byte)buf[off + 3] * 256;
			if (len == 0)
				break;
			filterSinglePkgHostOut(buf, off, len, rLen);
			// zr_log_debug_ec(0, "off = %d; len = %d; rLen=%d", off, len, rLen);
			if (off + len >= rLen)
				break;
			off += len;
			// zr_log_debug_ec(0, "continue off=%d", off);
		}
	}
	isShutdown = true;
}

void markUserLeave() {

	// Del gameuser slotRecord file when leaving
	BOOL res= DeleteFile(gamerSrPath);

	//Detect less users count
	// delay before check
	long delay = gamerPlayerId * 100;
	Sleep(delay);
	zr_log_debug_ec(0, "%s: Leave mark delay = %d", gamerUsername, delay);

	// check
	char unPath[MAX_PATH];
	sprintf(unPath, "%s.un", gamerIdPath);
	CopyFile(gamerIdPath, unPath, false);
	if (!DeleteFile(gamerIdPath))
		zr_log_error_ec(GetLastError(), "Del Failed! UnPath = %s", unPath);
}

bool gameResultCheck() {
	bool isGameResultSure = zr_fileVarBool_get("isGameResultSure");
	int gameResult = varClt->getInt("gameResult");
	//已知确定的结果
	if (isGameResultSure)
		return true;

	return false;

}

void gameResultCheckByMemScan() {
	int mapChecksum = getMapChecksum();
	zr_log_debug_ec(0, "gameResultCheckByMemScan mcs=%d", mapChecksum);
	switch (mapChecksum) {
	case MAP_CHKSUM_Z3_39D:
	case MAP_CHKSUM_Z3_42E:
	case MAP_CHESUM_DOTA_651_CN: {
		zr_log_debug_ec(0, "MemScan mapChecksum = %d", mapChecksum);
		int war3WinnerScanRes = waitForWar3WinnerScan();
		int addedWinTagCount =war3WinnerScanRes - beginWinnerScanRes;
		zr_log_debug_ec(0, "addedWinTagCount=%d", addedWinTagCount);
		if (addedWinTagCount > 0) {//蜀国Win
			if (hostTeamNum == 0) {//主机是蜀国 
				setGameResultVars(true, GAME_RES_HOST_WIN, 
				GAME_RES_REASON_FULL, "MemScan");
			} else {
				setGameResultVars(true, GAME_RES_HOST_LOST, 
				GAME_RES_REASON_FULL, "MemScan");
			}
		} else {//蜀国Lost
			if (hostTeamNum == 0) {//主机是蜀国 
				setGameResultVars(true, GAME_RES_HOST_LOST, 
				GAME_RES_REASON_FULL, "MemScan");
			} else {
				setGameResultVars(true, GAME_RES_HOST_WIN, 
				GAME_RES_REASON_FULL, "MemScan");
			}
		}
		break;
	}
	default:
		setGameResultVars(false, GAME_RES_UNKNOWN, 
		GAME_RES_REASON_UNKNOWN, "MemScan");
		break;
	}
}

void shutdownCheckThread(LPVOID lpParam) {
	while (true) {
		if (isShutdown) {
			closesocket(sock);
			closesocket(destSock);
			WSACleanup();
			break;
		}
		Sleep(200);
	}
	//	zr_log_debug("A pipe mission finished, waiting for threads shutdown...");
	while (true) {
		if (isShutdown)
			break;
		Sleep(200);
	}
	zr_log_debug_ec(0, "A TcpAdaptor over!  gamer=%s pid=%d tn=%d",
			gamerUsername, gamerPlayerId, gamerTeamNum);

	// 战斗没开始，退出
	if (!isFightBegin)
		return;

	// 标记用户离开（断开）
	markUserLeave();

	// 标记Team用户最后离开时间
	setTeamAllLeaveTime(gamerTeamNum);

	// 计算每队剩余用户数量
	hostTeamLess = varClt->getInt("hostTeamLess");
	otherTeamLess = varClt->getInt("otherTeamLess");
	if (gamerTeamNum == hostTeamNum) {
		hostTeamLess --;
		varClt->setInt("hostTeamLess", hostTeamLess);
	} else {
		otherTeamLess --;
		varClt->setInt("otherTeamLess", otherTeamLess);
	}
	zr_log_debug_ec(0, "hostTeamLess=%d, otherTeamLess=%d", hostTeamLess,
			otherTeamLess);

	//剩余用户
	lessGamerCount = getLessUserCount();
	zr_log_debug_ec(0, "Less=%d", lessGamerCount);

	//	// 主机队退人，不做全退判断
	//	if (gamerTeamNum != hostTeamNum) {
	//		//判断是否对方全退
	//		if (otherTeamLess <= 0) { //对方全退
	//			if (!zr_fileVarBool_get("isAllLeaveRes")) {
	//				//标记对方全退
	//				zr_fileVarBool_set("isAllLeaveRes", true);
	//				//记录退的时间
	//				varClt->setInt("allLeaveResTime", time(NULL));
	//				zr_log_debug_ec(0, "Team %d all leave",gamerTeamNum );
	//			}
	//		}
	//	}

	//不是最后一个用户, 游戏没结束
	if (lessGamerCount > 1) {
		return;
	}

	// 只进行１次GameOver处理
	//TODO:有冲突问题啊
	bool isGameOver = zr_fileVarBool_get("IsGameOver");
	zr_fileVarBool_set("IsGameOver", true);
	if (isGameOver) {
		zr_log_debug("Game has Already Over!");
		return;
	}

	// 记录游戏结束时间
	zr_log_debug_ec(0, " ----==== Game Over =====---- ");
	endTime = time(NULL);
	gameSpendSeconds = endTime - readyTime;
	zr_log_debug_ec(0, "Game Spend %d seconds.", gameSpendSeconds);

	// 不需要胜负判断的，存游戏，退出
	if (!isWinCheck()) {
		zr_log_debug("Need NOT wincheck");
		saveWar3Replay();
		return;
	}

	/****************************************************
	 * 以下是胜负判断的代码
	 * **************************************************/
	//延迟一会儿，等待各线程/进程将临时文件数据变量写完
	Sleep(500);

	//此时,是否胜负已定? 是则——报告,存盘,退出
	if (gameResultCheck()) {
		reportGameRes();
		saveWar3Replay();
		return;
	}

	//判断是否对方"主动"全退, isAllLeaveReasonLeave = true
	bool isAllLeaveReasonLeave = true;
	zr_log_debug("Begin check otherTeam All Leave...");
	for (int i=0; i<slotCount; i++) {
		if (srs[i].playerId == 0)
			continue;
		if (srs[i].teamNumber == hostTeamNum)
			continue;
		char username[128];
		getUsernameByPlayerId(srs[i].playerId, username);
		int leaveReason = getGamerLeaveReason(username);
		zr_log_debug_ec(0, "pid=%d username=%s leaveReason=%d",
				srs[i].playerId, username, leaveReason);
		if (LEAVE_REASON_LEAVE == leaveReason) {
			continue;
		} else {
			isAllLeaveReasonLeave = false;
		}
	}
	//对方全退，胜负明确——报告,存盘,退出
	if (isAllLeaveReasonLeave) {
		zr_log_debug_ec(0, "OtherTeam all leave reason=LEAVE!!!");
		setGameResultVars(true, GAME_RES_HOST_WIN, GAME_RES_REASON_LEAVE,
				"OtherTeam all leave reason=LEAVE!!!");

		reportGameRes();
		saveWar3Replay();
		return;
	}

	// 判断是否对方在游戏结束2秒前，全退，表示对方非正常退出，或者拔线
	int otherTeamNum = getOtherTeamNum();
	int otherTeamLeaveTime = getTeamAllLeaveTime(otherTeamNum);
	if (endTime - otherTeamLeaveTime >= 2) {
		zr_log_debug_ec(0, "OtherTeam all leave before 2s from gameOver.");
		setGameResultVars(true, GAME_RES_HOST_WIN, GAME_RES_REASON_LEAVE,
				"OtherTeam all before 2s from gameOver.");

		reportGameRes();
		saveWar3Replay();
		return;

	}

	int mapChecksum = getMapChecksum();
	switch (mapChecksum) {
	case MAP_CHESUM_DOTA_651_CN: {
		gameResultCheckByMemScan();
		reportGameRes();
		saveWar3Replay();
		break;
	}
	default: {
		// 判断是否主机退出
		if (isHostCancel) {
			setGameResultVars(true, GAME_RES_HOST_LOST, GAME_RES_REASON_LEAVE,
					"Host cancel");
			reportGameRes();
			saveWar3Replay();
			return;
		}

		gameResultCheckByMemScan();
		reportGameRes();
		saveWar3Replay();
		break;
	}
	}

	return;

}

void startTcpPipeAdaptor(char* pipeServerHost, int pipeServerPort,
		u_char* myAddr) {
	WSAData wsaData;
	u_short wVersionRequested= MAKEWORD( 2, 1);
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		zr_log_error_ec(err, "Socket error");
		return;
	}

	/**
	 * 连接pipe host
	 */
	sock=socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN soAddr;
	soAddr.sin_addr.S_un.S_addr = inet_addr(pipeServerHost);
	soAddr.sin_family=AF_INET;
	soAddr.sin_port=htons(pipeServerPort);
	zr_log_debug("connect to pipeServer");
	err = connect(sock, (SOCKADDR*)&soAddr, sizeof(SOCKADDR));
	if (SOCKET_ERROR == err) {
		zr_log_debug_ec(err, "connect failed.");
		return;
	}

	/**
	 * 向pipe host 发送初始数据包
	 * 1 个字节:角色标志 0x10
	 * 4 个字节:自己的虚拟IP
	 */
	char roleTag[1] = { 0x10 };
	send(sock, roleTag, 1, 0);
	send(sock, (char *)myAddr, 4, 0);

	/**
	 * 阻塞式接受TCP首先发的信息头数据，表示有客户端的连接请求透过pipe host到来了
	 * 32 字节：用户名
	 * 4 字节：安全Key
	 * 4 字节：自己的虚拟IP地址
	 * 2 字节：目标主机的虚拟端口号
	 * 之后  ：原本要发送的数据
	 * 这两个字节表示目标端口号 u_short 网络格式
	 */
	char Username[32];
	recv(sock, Username, 32, 0);
	zr_log_debug_ec(0, "%s enter my host", Username);
	strcpy(gamerUsername, Username);

	char skey[4];
	recv(sock, skey, 4, 0);

	u_short dest_nport;
	recv(sock, (char *)&dest_nport, 2, 0);

	/**
	 * 连向localhost，的dest port端口，建立socket通道
	 */
	destSock = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN soAddrDest;
	soAddrDest.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	soAddrDest.sin_family=AF_INET;
	soAddrDest.sin_port=dest_nport;
	zr_log_debug_ec(0, "connect to 127.0.0.1:%d", ntohs(dest_nport));
	err = connect(destSock, (SOCKADDR*)&soAddrDest, sizeof(SOCKADDR));
	if (SOCKET_ERROR == err) {
		zr_log_debug_ec(err, "connect failed.");
		return;
	}

	if (6112 == ntohs(dest_nport))
		isWar3 = true;

	// Set Socket NoDelay
	BOOL nodelay= TRUE;
	err = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&nodelay,
			sizeof nodelay);
	if (SOCKET_ERROR == err) {
		zr_log_error_ec(WSAGetLastError(),
				"setsockopt TCP_NODELAY failed. err=%d\r\n", err);
	}
	//	err = setsockopt(destSock, IPPROTO_TCP, TCP_NODELAY, (char *)&nodelay,
	//			sizeof nodelay);
	//	if (SOCKET_ERROR == err) {
	//		zr_log_error_ec(WSAGetLastError(),
	//				"setsockopt TCP_NODELAY failed. err=%d\r\n", err);
	//	}

	DWORD far threadID1, threadID2;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pipeThread1, NULL, 0,
	&threadID1);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pipeThread2, NULL, 0,
	&threadID2);
	shutdownCheckThread(NULL);
	//CreateThread(NULL,0,  (LPTHREAD_START_ROUTINE)shutdownCheckThread, NULL, 0, &checkThread);
}

void tcpAdaptorRun(LPVOID lpParam) {
	gf_props_t props;
	gf_props_read(&props);
	//	while (true) {
	isShutdown = false;
	startTcpPipeAdaptor(props.pipe_host, props.pipe_tcpport,
			props.v_ipaddr.u_bs);
	//		Sleep(1000);
	//	}
}

void startTcpAdaptorThread() {
	DWORD far threadID3;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)tcpAdaptorRun, NULL, 0, &threadID3);

}

void loggerSetting() {
	//Logger Setting
	zr_log_set_target(ZR_LOG_TG_FILE);
	char logpath[MAX_PATH];
	GetTempPath(sizeof logpath, logpath);
	strcat(logpath, "gf_tcp.log");
	zr_log_set_filepath(logpath);
}

void initFightDir() {
	GetTempPath(sizeof fightDirPath, fightDirPath);
	strcat(fightDirPath, "gf_fight");
	CreateDirectory(fightDirPath, NULL);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
		int nCmdShow) {
	varClt = new zr_sockVarClt("127.0.0.1", VARSRV_PORT);
	loggerSetting();
	initFightDir();

	gf_props_read(&props);

	tcpAdaptorRun(NULL);
	return 0;
}

int main() {
	varClt = new zr_sockVarClt("127.0.0.1", VARSRV_PORT);
	
	loggerSetting();
	initFightDir();

	props;
	gf_props_read(&props);

	tcpAdaptorRun(NULL);
	return 0;
}

