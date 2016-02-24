#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>

#include <zr_logger.h>

#include "gf_props.h"

#define PKG_LEN 36

bool test_recv() {
	int err;
	int local_port = 6112;
	u_short nport = htons(local_port);
	char *p_nport = (char *)&nport;

	// sockaddr_in - client
	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = nport;
	client.sin_addr.s_addr = INADDR_ANY;

	// sockaddr_in - server
	struct sockaddr_in server;
	server.sin_family = AF_INET;
//	server.sin_port = htons(6112);
	//	server.sin_addr.s_addr = inet_addr(dest_vip);
	server.sin_addr.s_addr = INADDR_ANY;

	// build socket 
	SOCKET sock;
	sock=socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == sock) {
		printf("socket create failed.\r\n", WSAGetLastError());
		return false;
	}
	
	int nZero=0;
	err =setsockopt(sock,SOL_SOCKET,SO_SNDBUF,(char *)&nZero,sizeof(nZero));
	err =setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(char *)&nZero,sizeof(nZero));
	if (SOCKET_ERROR == err) {
		zr_log_error_ec(WSAGetLastError(), "setsockopt SO_SNDBUF failed. err=%d\r\n", err);
		return 0;
	}

	err = bind(sock, (struct sockaddr*)&client, sizeof client);
	if (SOCKET_ERROR == err) {
		printf("Udp socket bind error[%d].\r\n", GetLastError());
		return false;
	}

	// recv
	char rbuf[PKG_LEN];
	int slen = sizeof server;
	while (true) {
		int rlen = recvfrom(sock, rbuf, PKG_LEN, 0, (struct sockaddr *)&server, &slen);
		if (SOCKET_ERROR == rlen) {
			zr_log_debug_ec(WSAGetLastError(), "recv failed");
		}

		printf("s=%d l=%d\r\n", rbuf[0], rlen);
		// send
		err
				= sendto(sock, rbuf, rlen, 0, (struct sockaddr*)&server,
						sizeof server);
		if (SOCKET_ERROR == err) {
			printf("Udp send error.\r\n");
		}
	}
	// close & clean
	closesocket(sock);

	return true;
}

bool test_send(char * dest_vip) {
	int err;
	int local_port = 6112;
	u_short nport = htons(local_port);
	char *p_nport = (char *)&nport;

	// sockaddr_in - client
	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = nport;
	client.sin_addr.s_addr = INADDR_ANY;

	// sockaddr_in - server
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(6112);
	server.sin_addr.s_addr = inet_addr(dest_vip);

	// build socket 
	SOCKET sock;
	sock=socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == sock) {
		printf("socket create failed.\r\n", WSAGetLastError());
		return false;
	}

	err = bind(sock, (struct sockaddr*)&client, sizeof client);
	if (SOCKET_ERROR == err) {
		printf("Udp socket bind error[%d].\r\n", GetLastError());
		return false;
	}

	// set timeout
	int timeout = 3000;
	//setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof timeout);
	int nZero=0;
	err =setsockopt(sock,SOL_SOCKET,SO_SNDBUF,(char *)&nZero,sizeof(nZero));
	err =setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(char *)&nZero,sizeof(nZero));
	if (SOCKET_ERROR == err) {
		zr_log_error_ec(WSAGetLastError(), "setsockopt SO_SNDBUF failed. err=%d\r\n", err);
		return 0;
	}
	
//	BOOL nodelay = TRUE;
//	err = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)nodelay, sizeof nodelay);
//	if (SOCKET_ERROR == err) {
//		zr_log_error_ec(WSAGetLastError(), "setsockopt TCP_NODELAY failed. err=%d\r\n", err);
//		return 0;
//	}
	// data package 
	char buf[PKG_LEN] = { 0 };
	char rbuf[PKG_LEN];
	
	for (int i=0; i<10; i++) {
		buf[0] = i;
		// send
		err = sendto(sock, buf, PKG_LEN, 0, (struct sockaddr*)&server, sizeof server);
		if (SOCKET_ERROR == err) {
			printf("Udp send error.\r\n");
		}

		// recv
		int slen = sizeof server;
		DWORD tmsend = timeGetTime();
		int rlen = recvfrom(sock, rbuf, PKG_LEN, 0, (struct sockaddr *)&server, &slen);
		if (SOCKET_ERROR == rlen) {
			int soErr = WSAGetLastError();
			zr_log_debug_ec(soErr, "recv failed");
			if (10060 == soErr)
				zr_log_debug_ec(soErr, "RECV TIMEOUT");
		} else {
			DWORD tmrecv = timeGetTime();
			printf("s=%d r=%d l=%d\r\n",tmrecv - tmsend, rbuf[0], rlen);
		}
	}

	closesocket(sock);

	return true;
}

int main(int argc, char ** argv) {
	if (argc < 2) {
		printf("miss args.");
		return 0;
	}

	int err;
	bool is_send = false;
	if (strcmp(argv[1], "send") == 0)
		is_send =true;

	char * dest_vip = argv[2];

	WSAData wsaData;
	u_short wVersionRequested= MAKEWORD( 2, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		zr_log_error_ec(err, "Socket error");
		return 0;
	}

	if (is_send) {
		printf("sending...\r\n");
		test_send(dest_vip);
	} else {
		printf("recving...\r\n");
		test_recv();
	}

	WSACleanup();

	return 0;
}