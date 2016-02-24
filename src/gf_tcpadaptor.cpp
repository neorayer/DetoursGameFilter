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

#include "gf_props.h"
#include "gf_head.h"
#include "gf_util.h"

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

//游戏结果未知
#define GAME_RES_UNKNOWN  0 
#define GAME_RES_HOST_WIN  1
#define GAME_RES_HOST_LOST  2
//游戏结果异常
#define GAME_RES_EXCEPTION  32

//游戏结果原因：未知
#define GAME_RES_REASON_UNKNOWN 0
//游戏结果原因：完胜
#define GAME_RES_REASON_FULL  1
//游戏结果原因：对手认输（提前退出）
#define GAME_RES_REASON_LEAVE  2

//int gameResultReason= GAME_RES_REASON_FULL;

//bool isGameResultSure = false;

//是否是Warcraft3游戏
bool isWar3 = false;

//本地Socket
SOCKET sock;
//目标Socket
SOCKET destSock;
//gf配置
gf_props_t props;

//是否结束标志
bool isShutdown = false;
//是否是主机退出
bool isHostCancel = false;

//玩家用户名
char gamerUsername[32];
//玩家队伍编号
byte gamerTeamNum = -1;
//玩家ID
byte gamerPlayerId = -1;
//队伍总数
int teamCount = 0;

//Slot位置总数
int slotCount = -1;
//Slot_record数据组
t_slot_record srs[20];

//int hTeamCount = 0;
//int oTeamCount = 0;

//byte hostTeamNum;

//char hostTeamS[512];
//char otherTeamS[512];

// ?
char fightDirPath[MAX_PATH];
// ?
char gamerSrPath[MAX_PATH];
// ?
char gamerIdPath[MAX_PATH];
// 战斗是否开始
BOOL isFightBegin = false;
//int hostTeamLess = 0;
//int otherTeamLess = 0;
//int less;

//游戏开始时间
int beginTime = 0;
//游戏准备好的时间
int readyTime = 0;
//游戏结束的时间
int endTime = 0;
//游戏总体花费的时间（秒）
int gameSpendSeconds = 0;

//离开原因：未知
#define LEAVE_REASON_UNKNOWN 0
//离开原因：主动离开
#define LEAVE_REASON_LEAVE 1
//离开原因：因胜利了自动离开
#define LEAVE_REASON_WIN 2
//离开原因：因失败了自动离开
#define LEAVE_REASON_LOST 3
//int gamerLeaveReason= LEAVE_REASON_UNKNOWN;


//F变量写入：地图交验码
void setMapChecksum(int mapCheckSum) {
	zr_fileVarInt_set("MapChecksum", mapCheckSum);
}
//F变量读取：地图交验码
int getMapChecksum() {
	return zr_fileVarInt_get("MapChecksum");
}

//F变量写入：玩家ID与username的对应
void setUsernameByPlayerId(byte id, char * name) {
	char unPath[MAX_PATH];
	sprintf(unPath, "%s\\%d.un", fightDirPath, id);
	zr_str2file(name, unPath);
	zr_log_debug_ec(0, "user:%s  playerId=%d", name, id);
}

//F变量读取：通过ID获取username
void getUsernameByPlayerId(byte id, char * buf) {
	char unPath[MAX_PATH];
	sprintf(unPath, "%s\\%d.un", fightDirPath, id);
	zr_file2strbuf(unPath, buf);
}

//结构：队伍信息
typedef struct TeamInfo {
	//成员总数
	byte gamersCount;
	//成员还剩余（未离开的）
	byte gamersLess;
	//游戏名称？
	char gameNames[512];
} t_TeamInfo;

//队伍信息数组
TeamInfo teamInfos[20];
//队伍总数
TeamInfo teamsCount;

//解析队伍信息：到teamInfos,teamsCount
int parseTeamInfos() {
	int i=0;
	int teamsCount = 0;
	for (i=0; i<20; i++) {
		teamInfos[i].gamersCount = 0;
		teamInfos[i].gamersLess = 0;
		teamInfos[i].gameNames[0] = '\0';
	}
	for (i=0; i<12; i++) {
		byte tn = srs[i].teamNumber;

		// 构造对战双方人员字串
		byte playerId = srs[i].playerId;
		if (playerId < 1) // skip computer gamer
			continue;
		char username[128];
		if (playerId == 1) {
			strcpy(username, props.username);
		} else {
			getUsernameByPlayerId(playerId, username);
		}
		strcat(teamInfos[tn].gameNames, username);
		strcat(teamInfos[tn].gameNames, ":");

		teamInfos[tn].gamersCount ++;
		teamInfos[tn].gamersLess = teamInfos[tn].gamersCount;
	}

	//计算战队总数
	for (i=0; i<20; i++) {
		if (i>=12)
			continue;
		if (teamInfos[i].gamersCount <= 0)
			continue;
		teamsCount ++;
	}
	return teamsCount;
}

//错误标志：队伍编号错误（异常）
#define ERR_TEAM_NUM 100
//获取主机队伍编号
byte getHostTeamNum() {
	for (int i=0; i<slotCount; i++) {
		if (srs[i].playerId == 1)
			return srs[i].teamNumber;
	}
	return ERR_TEAM_NUM;
}
byte getOtherTeamNum() {
	int i=0;
	int hostTeamNum = getHostTeamNum();
	for (i=0; i<20; i++) {
		if (teamInfos[i].gamersCount <= 0)
			continue;
		if (12 == i)
			continue;
		if (i != hostTeamNum)
			return i;
	}
	return ERR_TEAM_NUM;
}

//记录队伍信息日志
void debugTeamInfos() {
	int i=0;
	for (i=0; i<20; i++) {
		if (teamInfos[i].gamersCount <= 0)
			continue;
		zr_log_debug_ec(0, "tn=%i, games=%s, count=%d, less=%d", i,
				teamInfos[i].gameNames, teamInfos[i].gamersCount,
				teamInfos[i].gamersLess);
		if (getHostTeamNum() == i)
			zr_log_debug_ec(0, "It's host team");
	}
}

//F变量写入：游戏结果，胜利队伍编号，胜利原因，描述
void setGameResultVars(bool isGameResultSure, int gameResult,
		int gameResultReason, char * desc) {
	zr_log_debug_ec(0, "setGameResultVars(%d, res=%d, reason=%d): desc=%s",
			isGameResultSure, gameResult, gameResultReason, desc);
	zr_fileVarInt_set("gameResult", gameResult);
	zr_fileVarBool_set("isGameResultSure", isGameResultSure);
	zr_fileVarInt_set("gameResultReason", gameResultReason);
}

//F变量写入：玩家离开原因
void setGamerLeaveReason(char *username, int leaveReason) {
	char buf[128];
	sprintf(buf, "LeaveReason_%s", username);
	zr_fileVarInt_set(buf, leaveReason);
	zr_log_debug_ec(0, "setGamerLeaveReason %s=%d", username, leaveReason);
}

//F变量读取：玩家离开原因
int getGamerLeaveReason(char *username) {
	char buf[128];
	sprintf(buf, "LeaveReason_%s", username);
	return zr_fileVarInt_get(buf);
}

//F变量写入：队伍完全离开时间
void setTeamAllLeaveTime(int teamNum) {
	char varName[128];
	sprintf(varName, "team_%d", teamNum);
	zr_fileVarInt_set(varName, time(NULL));
}

//F变量读取：队伍完全离开时间
int getTeamAllLeaveTime(int teamNum) {
	char varName[128];
	sprintf(varName, "team_%d", teamNum);
	return zr_fileVarInt_get(varName);
}

//等待游戏结果内存扫描
int waitForWar3WinnerScan() {
	BOOL isBeginScan = zr_fileVarBool_get("IsBeginScan");

	zr_log_debug("waitForWar3WinnerScan");
	char buf[128];
	sprintf(buf, "0");
	zr_sock_startup();
	ZrSocket * zrSocket = new ZrSocket();
	if (!zrSocket->connect("127.0.0.1", 8497)) {
		zr_log_error_ec(WSAGetLastError(), "connect 127.0.0.1:8497 Failed");
		return 0;
	}

	if (isBeginScan) {
		zr_log_debug_ec(0, "Begin War3WinnerScan Over.");
		return 0;
	}

	int rLen = zrSocket->recv(buf, 128);
	if (rLen < 0) {
		zr_log_error_ec(WSAGetLastError(), "access Socket recv Error.");
		return 0;
	}
	zr_log_debug_ec(0, "Get War3WinnerScanRes = %s", buf);
	int res = atoi(buf);
	zrSocket->close();
	zr_sock_cleanup();
	delete zrSocket;

	return res;
}

bool isPointableMap(int mapChecksum) {
	switch (mapChecksum) {
	case MAP_CHKSUM_Z3_39D:
	case MAP_CHKSUM_Z3_39D_CREG:
	case MAP_CHKSUM_Z3_39D_SOLO:
	case MAP_CHKSUM_Z3_42E:

	case MAP_CHKSUM_ORC3C_169V:
	case MAP_CHESUM_ORC3C_184_FIX2:
	case MAP_CHESUM_ORC3C_185_L7:
	case MAP_CHESUM_ORC3C_185_B1:

	case MAP_CHESUM_DOTA_651_CN:
	case MAP_CHESUM_DOTA_653_CN:

	case MAP_CHKSUM_LST_1:
	case MAP_CHKSUM_LST_2:
	case MAP_CHKSUM_ECHO_ISLES_1:
	case MAP_CHKSUM_TERENAS_STAND_1:
	case MAP_CHKSUM_ISLANDS_1:
	case MAP_CHKSUM_TURTLE_ROCK_1:
	case MAP_CHKSUM_TURTLE_ROCK_2:
	case MAP_CHESUM_TWISTED_MEADOWS_1:
	case MAP_CHESUM_GNOLL_WOOD_1: {
		return true;
	}
	default: {
		zr_log_debug_ec(0, "It is not winCheck map checksum=%d", mapChecksum);
		return false;
		break;
	}
	}	
}
//是否需要胜负判断
bool isWinCheck() {
	int mapChecksum = getMapChecksum();
	if (!isPointableMap(mapChecksum)) {
		return false;
	}


	// 判定队伍数量
	if (teamCount != 2) {
		zr_log_debug_ec(0, "NOT winner Check becouse teamCount=%d", teamCount);
		return false;
	}

	//人数判定
	//	zr_log_debug_ec(0, "Mode %d vs %d", hTeamCount, oTeamCount);
	//	if (hTeamCount != oTeamCount) {
	//zr_log_debug_ec(0, "NOT winner Check becouse user count NOT balance.");
	// return false;
	//	}
	return true;
}

/**
 * 游戏结果
 */
class FightResult {
public:
	FightResult();
	void setHostGamers(char * str, int tn, char * hostUsername);
	void setOtherGamers(char * str, int tn);
	void toUrlParams(char *buf);

	bool isResSure;
	int res;

	char hostUsername[64];
	char hostGamers[256];
	char otherGamers[256];
	int hostTn;
	int otherTn;

	int gameSeconds;
	int mapChecksum;
	int resReason;
};

FightResult::FightResult() {
	isResSure = false;
	res = GAME_RES_UNKNOWN;

	sprintf(hostUsername, "");
	sprintf(hostGamers, "");
	sprintf(otherGamers, "");
	hostTn = -1;
	otherTn = -1;

	gameSeconds = -1;
	mapChecksum = 0;
	resReason = GAME_RES_REASON_UNKNOWN;
}
void FightResult::toUrlParams(char *buf) {
	sprintf(
			buf,
			"isressure=%d&res=%d&hostusername=%s&hostgamers=%s&othergamers=%s&hosttn=%d&othertn=%d&gameseconds=%d&mapchecksum=%d&resreason=%d",
			isResSure, res, hostUsername, hostGamers, otherGamers, hostTn,
			otherTn, gameSeconds, mapChecksum, resReason);
}
void FightResult::setHostGamers(char * str, int tn, char * hun) {
	sprintf(hostGamers, "%s", str);
	hostTn = tn;
	sprintf(hostUsername, "%s", hun);
}

void FightResult::setOtherGamers(char * str, int tn) {
	sprintf(otherGamers, "%s", str);
	otherTn = tn;
}

void buildGameResultStr(char * resStr) {
	int hostTn = getHostTeamNum();
	int otherTn = getOtherTeamNum();

	FightResult * fr = new FightResult();
	fr->isResSure = zr_fileVarBool_get("isGameResultSure");
	fr->res = zr_fileVarInt_get("gameResult");

	fr->setHostGamers(teamInfos[hostTn].gameNames, hostTn, props.username);
	fr->setOtherGamers(teamInfos[otherTn].gameNames, otherTn);

	fr->mapChecksum = getMapChecksum();
	fr->gameSeconds = gameSpendSeconds;
	fr->resReason = zr_fileVarInt_get("gameResultReason");

	fr->toUrlParams(resStr);

}

void reportGameRes() {

	char resStr[1024];
	buildGameResultStr(resStr);

	char url[1024];

	sprintf(url, "%s?&username=%s&sessionKey=%s&roomid=%s&%s", props.res_url,
			props.username, props.sessionkey, props.roomid, resStr);
	zr_log_debug_ec(0, "report res %s", url);
	zr_sock_startup();
	zr_http_request(url, NULL);
	zr_sock_cleanup();
}

//获得玩家总数
int getUsersCount() {
	int c =0;
	for (int i=0; i<slotCount; i++) {
		if (srs[i].playerId > 0)
			c++;
	}
	return c;
}

//获得剩余玩家总数
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

//保存所有slotRecord记录
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

//void secondOutBan(char * gamerUsername) {
//	char buf[256];
//	sprintf(buf, "SOBAN_%s", gamerUsername);
//	zr_log_debug_ec(0, "user %s SECOND OUT, BAN!!", gamerUsername);
//	zr_fileVarInt_set(buf, time(NULL));
//}
//
//bool isSecondOutBan(char * gamerUsername) {
//	char buf[256];
//	sprintf(buf, "SOBAN_%s", gamerUsername);
//	int t = zr_fileVarInt_get(buf);
//	if (t <= 0)
//		return false;
//	if (time(NULL) - t > 600)
//		return true;
//	return false;
//}

//TCP数据包过滤器：进入主机数据
void inline filterSinglePkgHostIn(char * buf, char offSet /*The pos of 0xf7*/,
		int onePkgLen, int len) {
	byte actTag = (byte)buf[offSet + 1];
	// zr_log_debug_ec(0, "filter actTag=%d", actTag);
	switch (actTag) {
	case 0x21: { // gamer quit
		byte reasonTag = (byte)buf[offSet + 4];
		zr_log_debug_ec(0, "gamer  %s  send leave tag=%d", gamerUsername,
				reasonTag);
		if (0x07 == reasonTag) {
			//用户主动离开，标记用户离开原因
			setGamerLeaveReason(gamerUsername, LEAVE_REASON_LEAVE);
			zr_log_debug_ec(0, "%s tn=%d quit", gamerUsername, gamerTeamNum);
			if (isFightBegin && (beginTime - time(NULL) < 3)) {
				//secondOutBan(gamerUsername);
			}
		} else if (0x09 == reasonTag) {
			if (gamerTeamNum != 12)
				zr_log_debug_ec(0, "tn=%d win", gamerTeamNum);
		} else if (0x08 == reasonTag) {
			if (gamerTeamNum != 12)
				zr_log_debug_ec(0, "tn=%d lost", gamerTeamNum);
		}
		break;
	}
	default:
		break;
	}
}

//管道线程：服务器=>主机
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
				//zr_log_debug_ec(0, "%d != 0xf7", (byte)buf[off]);
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

//TCP数据包过滤器：主机发出数据
void inline filterSinglePkgHostOut(char * buf, char offSet /*The pos of 0xf7*/,
		int onePkgLen, int len) {
	byte actTag = (byte)buf[offSet + 1];
	//zr_log_debug_ec(0, "filter actTag=%d", actTag);
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

		//内存扫描
		int mapChecksum = getMapChecksum();
		switch (mapChecksum) {
		case MAP_CHKSUM_Z3_39D:
		case MAP_CHKSUM_Z3_39D_CREG:
		case MAP_CHKSUM_Z3_42E:
		case MAP_CHKSUM_Z3_39D_SOLO:
		case MAP_CHESUM_DOTA_651_CN:
		case MAP_CHESUM_DOTA_653_CN: {
			if (01 == (byte)buf[offSet + 4]) { //会发出两08标记包，选择其中附加数值是1的那个
				zr_fileVarBool_set("IsBeginScan", true);
				int beginWinnerScan = waitForWar3WinnerScan();
				zr_log_debug_ec(0, "begin winner scan=%d", beginWinnerScan);
			}
			break;
		}
		default:
			break;
		}

		break;
	}
	case 0x0b: {
		zr_log_debug(" ---=== Fight Begin (Init)===---");
		int i=0;

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

		//解析srs，填充TeamInfos[20]全局变量
		teamCount = parseTeamInfos();
		debugTeamInfos();

		//hostTeamNum = getHostTeamNum();
		//zr_log_debug_ec(0, "HostTeamNum = %d", hostTeamNum);


		// set gameTeamNum
		for (i=0; i<slotCount; i++) {
			if (srs[i].playerId <=0) // skip computer player
				continue;
			if (srs[i].playerId == gamerPlayerId)
				gamerTeamNum = srs[i].teamNumber;
		}

		// Init TeamAllLeave time
		for (i=0; i<20; i++) {
			if (teamInfos[i].gamersCount <=0)
				continue;
			//标记队伍离开时间
			setTeamAllLeaveTime(i);

			//写入各队剩余人数
			char varName[128];
			sprintf(varName, "teamGamesLess.%d", i);
			zr_fileVarInt_set(varName, teamInfos[i].gamersLess);
		}

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
		zr_log_debug("Host got a 0x07 actTag");
		char winTag = buf[offSet + 5];
		byte playerId = (byte)buf[offSet +4];
		if (0x07 == winTag) { // Host Cancel
			if (playerId == 1) {
				isHostCancel = true;
				zr_log_debug("set HostCancel true");
			}
		} else if (0x08 == winTag) {// This adaptor gamer win
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
			case MAP_CHKSUM_TURTLE_ROCK_2:
			case MAP_CHESUM_TWISTED_MEADOWS_1:
			case MAP_CHESUM_GNOLL_WOOD_1:
				if (gamerTeamNum != 12)
					zr_log_debug_ec(0, "tn=%d lost", gamerTeamNum);
				zr_log_debug("I am sure the game result, from standard map");
				setGameResultVars(true, GAME_RES_HOST_LOST, 
				GAME_RES_REASON_FULL, "Host send 0x08 && match MapChecksum");
				break;
			default:
				break;
			}
		} else if (0x09 == winTag) { // This adaptor gamer lost
			if (gamerTeamNum != 12)
				zr_log_debug_ec(0, "tn=%d lost1", gamerTeamNum);
			if (getHostTeamNum() == gamerTeamNum)
				setGameResultVars(true, GAME_RES_HOST_LOST, 
				GAME_RES_REASON_FULL, "Host send 0x09");
			else
				setGameResultVars(true, GAME_RES_HOST_WIN, 
				GAME_RES_REASON_FULL, "Host send 0x09");
		}
		break;
	}
	case 0x09: {//SlotRecords
		slotCount = buf[offSet + 6];
		for (int i=0; i<slotCount; i++) {
			memcpy(&(srs[i]), (void *)(buf + offSet + 7 + i * 9),
					sizeof t_slot_record);

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

//管道线程：主机=>服务器
void pipeThread2(LPVOID lpParam) {
	char buf[4096];
	isShutdown = false;
	while (true) {
		// 0xf7
		if (recv(destSock, buf, 1, 0) <= 0) {
			break;
		}
		if (0xf7 != (byte)buf[0]) {
			break;
		}
		// cmdTag
		if (recv(destSock, buf + 1, 1, 0) <= 0) {
			break;
		}
		// len
		if (recv(destSock, buf + 2, 2, 0) <=0) {
			break;
		}
		int len = (byte)buf[2] + (byte)buf[3] * 256;
		// zr_log_debug_ec(0, "%02x %02x len=%d", (byte)buf[0], (byte)buf[1], len);
		if (len - 4 > 0) {
			if (recv(destSock, buf + 4, len - 4, 0) <=0) {
				break;
			}
		}
		filterSinglePkgHostOut(buf, 0, len, len);
		send(sock, buf, len, 0);
	}
	int lessReadLen = 0;
	//	while (true) {
	//		int rLen = recv(destSock, buf, 4096, 0);
	//		if (rLen<=0)
	//			break;
	//		send(sock, buf, rLen, 0);
	//
	//		int off = lessReadLen;
	//		zr_log_debug_ec(0, "off=%d", off);
	//		for (;;) {
	//			zr_log_debug_ec(0, "%02x %02x", buf[off], buf[off+1]);
	//			if ((byte)buf[off] != 0xf7) {
	//				zr_log_debug_ec(0, "%d != 0xf7 bufrLen=%d", (byte)buf[off],
	//						rLen);
	//				return;
	//				break;
	//			}
	//			int len = (byte)buf[off + 2] + (byte)buf[off + 3] * 256;
	//			//zr_log_debug_ec(0, "%02x %02x %02x %02x %d", (byte)buf[off], (byte)buf[off+1], (byte)buf[off+2], (byte)buf[off + 3], len);
	//			if (len <= 0) {
	//				zr_log_debug_ec(0, "pkg error!!! len = %d", len);
	//				break;
	//			}
	//			filterSinglePkgHostOut(buf, off, len, rLen);
	//			// zr_log_debug_ec(0, "off = %d; len = %d; rLen=%d", off, len, rLen);
	//			lessReadLen = off + len - rLen;
	//
	//			if (lessReadLen >= 0) {
	//				zr_log_debug_ec(0, "lessReadLen = %d", lessReadLen);
	//				break;
	//			}
	//			off += len;
	//		}
	// zr_log_debug_ec(0, "continue off=%d", off);
	//	}
	isShutdown = true;
}

//标记玩家离开
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

//检查是否游戏结果已经确定
bool gameResultCheck() {
	bool isGameResultSure = zr_fileVarBool_get("isGameResultSure");
	//已知确定的结果
	if (isGameResultSure)
		return true;

	return false;

}

//循环检测线程：用于某用户的管道是否结束（通过isShutdown变量）
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
	for (int i=0; i<20; i++) {
		if (teamInfos[i].gamersCount <= 0)
			continue;
		if (gamerTeamNum == i) {
			char varName[128];
			sprintf(varName, "teamGamesLess.%d", i);
			teamInfos[i].gamersLess = zr_fileVarInt_get(varName);
			teamInfos[i].gamersLess --;
			zr_fileVarInt_set(varName, teamInfos[i].gamersLess);
		}
	}
	debugTeamInfos();

	//剩余用户
	int less = getLessUserCount();
	zr_log_debug_ec(0, "Less=%d", less);

	int hostTeamNum = getHostTeamNum();
	// 主机队退人，不做全退判断
	if (gamerTeamNum !=hostTeamNum) {
		//判断是否对方全退
		if (teamInfos[gamerTeamNum].gamersLess <= 0) { //对方全退
			if (!zr_fileVarBool_get("isAllLeaveRes")) {
				//标记对方全退
				zr_fileVarBool_set("isAllLeaveRes", true);
				//记录退的时间
				zr_fileVarInt_set("allLeaveResTime", time(NULL));
				zr_log_debug_ec(0, "Team %d all leave", gamerTeamNum);
			}
		}
	}

	//不是最后一个用户, 游戏没结束
	if (less > 1) {
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
	gameSpendSeconds = 0 == readyTime ? 0 : endTime - readyTime;
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

	byte otherTeamNum = getOtherTeamNum();

	// 判断是否对方在游戏结束2秒前，全退，表示对方 非正常退出，或者拔线
	// 此时除了主机队还有其他人，才有效
	int otherTeamLeaveTime = getTeamAllLeaveTime(otherTeamNum);
	if (endTime - otherTeamLeaveTime >= 2) {
		zr_log_debug_ec(0, "OtherTeam all leave before 2s from gameOver.");
		setGameResultVars(true, GAME_RES_HOST_WIN, GAME_RES_REASON_LEAVE,
				"OtherTeam all before 2s from gameOver.");

		reportGameRes();
		saveWar3Replay();
		return;
	}

	//根据特定游戏做内存扫描判断
	int mapChecksum = getMapChecksum();
	switch (mapChecksum) {
	case MAP_CHKSUM_Z3_39D_CREG:
	case MAP_CHKSUM_Z3_39D:
	case MAP_CHKSUM_Z3_39D_SOLO:
	case MAP_CHESUM_DOTA_653_CN:
	case MAP_CHESUM_DOTA_651_CN: {
		zr_fileVarBool_set("IsBeginScan", false);
		int winRes = waitForWar3WinnerScan();
		zr_log_debug_ec(0, "endWinnerScanRes=%d ", winRes);
		int gameRes= GAME_RES_UNKNOWN;
		switch (winRes) {
		case WINNER_HOST:
			setGameResultVars(true, GAME_RES_HOST_WIN, GAME_RES_REASON_FULL,
					"z3 mem scan");
			break;
		case WINNER_CLIENT:
			setGameResultVars(true, GAME_RES_HOST_LOST, GAME_RES_REASON_FULL,
					"z3 mem scan");
			break;
		case WINNER_TN_0:
			setGameResultVars(true, hostTeamNum == 0 ? GAME_RES_HOST_WIN
					: GAME_RES_HOST_LOST, 
			GAME_RES_REASON_FULL, "z3 mem scan");
			break;
		case WINNER_TN_1:
			setGameResultVars(true, hostTeamNum == 1 ? GAME_RES_HOST_WIN
					: GAME_RES_HOST_LOST, GAME_RES_REASON_FULL, "z3 mem scan");
			break;
		case WINNER_NONE:
		default:
			if (isHostCancel) {//主机主动提前退出
				setGameResultVars(true, GAME_RES_HOST_LOST, 
				GAME_RES_REASON_LEAVE, "host cancel");
			} else if (teamInfos[otherTeamNum].gamersLess == 0) { //判断是否对方"主动"全退
				zr_log_debug_ec(0, "OtherTeam all leave reason=LEAVE!!!");
				setGameResultVars(true, GAME_RES_HOST_WIN, 
				GAME_RES_REASON_LEAVE, "OtherTeam all leave reason=LEAVE!!!");
			} else {
				setGameResultVars(true, GAME_RES_UNKNOWN, 
				GAME_RES_REASON_UNKNOWN, "z3 mem scan");
			}
			break;
		}

		reportGameRes();
		saveWar3Replay();
		break;
	}
	default: {
		// 判断是否主机退出
		if (isHostCancel) {
			setGameResultVars(true, GAME_RES_HOST_LOST, 
			GAME_RES_REASON_LEAVE, "Host cancel");
			reportGameRes();
			saveWar3Replay();
			return;
		}else {
			setGameResultVars(true, GAME_RES_HOST_WIN, 
			GAME_RES_REASON_LEAVE, "Others all leave");
			reportGameRes();
			saveWar3Replay();
			return;
		}
		saveWar3Replay();
		break;
	}
	}

	return;

}

//开启TCP管道适配器
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

	char clientId[4];
	zr_file2memBuf("dst_clientId", clientId, 4);
	zr_log_debug_ec(0, "try to connect dst_client %d %d %d %d", clientId[0],
			clientId[1], clientId[2], clientId[3]);
	send(sock, clientId, 4, 0);

	/**
	 * 阻塞式接受TCP首先发的信息头数据，表示有客户端的连接请求透过pipe host到来了
	 * 32 字节：用户名
	 * 4 字节：安全Key
	 * 2 字节：目标主机的虚拟端口号
	 * 
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
	err = setsockopt(destSock, IPPROTO_TCP, TCP_NODELAY, (char *)&nodelay,
			sizeof nodelay);
	if (SOCKET_ERROR == err) {
		zr_log_error_ec(WSAGetLastError(),
				"setsockopt TCP_NODELAY failed. err=%d\r\n", err);
	}

	DWORD far threadID1, threadID2;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pipeThread1, NULL, 0,
	&threadID1);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pipeThread2, NULL, 0,
	&threadID2);
	shutdownCheckThread(NULL);
	//CreateThread(NULL,0,  (LPTHREAD_START_ROUTINE)shutdownCheckThread, NULL, 0, &checkThread);
}

//TCP管道运行时线程函数
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

//开启TCP管道线程
void startTcpAdaptorThread() {
	DWORD far threadID3;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)tcpAdaptorRun, NULL, 0, &threadID3);
}

//日志设定
void loggerSetting() {
	//Logger Setting
	zr_log_set_target(ZR_LOG_TG_FILE);
	char logpath[MAX_PATH];
	GetTempPath(sizeof logpath, logpath);
	strcat(logpath, "gf_tcp.log");
	zr_log_set_filepath(logpath);
}

//?
void initFightDir() {
	GetTempPath(sizeof fightDirPath, fightDirPath);
	strcat(fightDirPath, "gf_fight");
	CreateDirectory(fightDirPath, NULL);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
		int nCmdShow) {
	loggerSetting();
	initFightDir();

	gf_props_read(&props);

	tcpAdaptorRun(NULL);
	return 0;
}

int main() {
	loggerSetting();
	initFightDir();

	props;
	gf_props_read(&props);

	tcpAdaptorRun(NULL);
	return 0;
}

