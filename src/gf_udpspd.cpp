#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#include<iostream>
#include<map>
#include<string>
#include<iterator>

#include <zr_logger.h>
#include <zr_file.h>
#include <zr_str.h>
#include <zr_sock.h>

#include "gf_props.h"

using namespace std;

map<string, int> hostspd_map;

#ifndef MAX_IP
#define MAX_IP 18
#endif

#ifndef MAX_ROOM_NAME
#define MAX_ROOM_NAME 128
#endif

typedef struct room_spd_item {
	char roomname[MAX_ROOM_NAME];
	int roomaddr;
	char hostip[MAX_IP];
	int port;
	int delay;
} room_spd_item_t;

#define MAX_ROOMS	1000

/**
 * Return
 * 	rooms count
 */
int spd_read_rooms(char * path, room_spd_item_t spd_items[]) {
	hostspd_map.empty();
	
	for (int i=0; i<MAX_ROOMS; i++) {
		strcpy(spd_items[i].hostip, "10.1.1.196");
		spd_items[i].port = 8000;
	}

	// read file
	HANDLE fh= CreateFile (path, GENERIC_READ,FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (INVALID_HANDLE_VALUE == fh) {
		zr_log_error_ec(GetLastError(), "open file failed. %s", path);
		return false;
	}
	int buf_len = 1024000;
	char * buf = (char *)malloc(buf_len);
	DWORD rlen = 0;
	ReadFile(fh, buf, buf_len, &rlen, NULL);
	CloseHandle(fh);
	buf[rlen] = '\0';

	// parse file
	char *ss[MAX_ROOMS];
	int sc = 0;
	ss[sc] = buf;
	int off = 0;
	while (off < rlen) {
		if ('\r' == buf[off]) {
			buf[off] = '\0';
			off += 2;
			ss[++sc] = buf + off;
		}
		off ++;
	}
	// parse line
	int room_count = 0;
	for (i=0; i<sc; i++) {
		room_spd_item_t * pitem = &(spd_items[room_count]);
		//		printf("%s\r\n", ss[i]);

		// room name
		char * rname_pos = ss[i];
		char * pos = strchr(rname_pos, '=');
		if (pos < 0)
			continue;
		*pos = '\0';
		strcpy(spd_items[i].roomname, rname_pos);
		//		printf("roomname=%s\r\n", pitem->roomname);

		// hostip
		pos++;
		char * ip_pos = pos++;
		pos = strchr(pos, '|');
		if (pos < 0)
			continue;
		*pos = '\0';
		strcpy(spd_items[i].hostip, ip_pos);
		//		printf("hostip=%s\r\n", pitem->hostip);

		//port
		pos++;
		char * port_pos = pos++;
		pos = strchr(pos, '|');
		if (pos < 0)
			continue;
		*pos = '\0';
		char port_buf[64];
		strcpy(port_buf, port_pos);
		pitem->port = atoi(port_buf);
		//		printf("port=%d\r\n", pitem->port);

		pitem->delay = -1;

		room_count++;
	}

	// free buf
	free(buf);

	return room_count;
}

#define PKG_LEN 15
bool send_test(char * hostip, int port, int * delay) {
	*delay = -1;

	int err;

	// sockaddr_in - server
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(hostip);

	// build socket 
	SOCKET sock;
	sock=socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == sock) {
		printf("socket create failed.\r\n", WSAGetLastError());
		return false;
	}

	// set timeout
	int timeout = 1000;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof timeout);

	// data package 
	char buf[PKG_LEN] = { 0 };

	buf[0] = 's';
	buf[1] = 'p';
	buf[2] = 'e';
	buf[3] = 'd';
	// send
	err = sendto(sock, buf, PKG_LEN, 0, (struct sockaddr*)&server,
			sizeof server);
	if (SOCKET_ERROR == err) {
		printf("Udp send error.\r\n");
		return false;
	}

	// recv
	char rbuf[PKG_LEN];
	int slen = sizeof server;
	DWORD tmsend = timeGetTime();
	int rlen = recvfrom(sock, rbuf, PKG_LEN, 0, (struct sockaddr *)&server,
			&slen);
	if (SOCKET_ERROR == rlen) {
		int soErr = WSAGetLastError();
		zr_log_debug_ec(soErr, "recv failed");
		if (10060 == soErr)
			zr_log_debug_ec(soErr, "RECV TIMEOUT");
	} else {
		DWORD tmrecv = timeGetTime();
		*delay = tmrecv - tmsend;
	}

	closesocket(sock);

	return true;
}

bool spd_save(char * path, room_spd_item_t spd_items[], int roomc) {
	for (int i=0; i<roomc; i++) {
		room_spd_item_t * pitem = &(spd_items[i]);
		char key[MAX_IP + 10];
		sprintf(key, "%s:%d", pitem->hostip, pitem->port);
		if (hostspd_map.count(key)) {
			pitem->delay =hostspd_map[key];
			continue;
		}
		send_test(pitem->hostip, pitem->port, &(pitem->delay));
		hostspd_map[key] = pitem->delay;
		printf("test once\r\n");
	}

	HANDLE fh= CreateFile (path, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (INVALID_HANDLE_VALUE == fh) {
		zr_log_error_ec(GetLastError(), "open file failed. %s", path);
		return false;
	}
	DWORD wlen = 0;
	char * section = "[RoomSpeed]\r\n";
	WriteFile(fh, section, strlen(section), &wlen, NULL);
	for (i=0; i<roomc; i++) {
		room_spd_item_t * pitem = &(spd_items[i]);
//		char key[MAX_IP + 10];
//		sprintf(key, "%s:%d", pitem->hostip, pitem->port);

//		if (hostspd_map.count(pitem->hostip))
//			continue;
//		int delay = hostspd_map[pitem->hostip];
//		if (-1 == delay) {
//			send_test(pitem->hostip, pitem->port, &(pitem->delay));
//			hostspd_map[pitem->hostip] = pitem->delay;
//		}
//		
		char line_buf[256];
		sprintf(line_buf, "%s=%d\r\n", pitem->roomname, pitem->delay);
		WriteFile(fh, line_buf, strlen(line_buf), &wlen, NULL);
	}

	CloseHandle(fh);
	return true;
}

bool spd_test(char * rooms_path) {
	int err =0;
	
	room_spd_item_t spd_items[MAX_ROOMS];
	int roomc = spd_read_rooms(rooms_path, spd_items);
	spd_save(rooms_path, spd_items, roomc);

	return true;

}

//int main(int argc, char ** argv) {
//	spd_test(argv[1]);
//	
//	return 0;
//}