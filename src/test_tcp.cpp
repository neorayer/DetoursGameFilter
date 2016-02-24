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

	// sockaddr_in - server
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = nport;
	server.sin_addr.s_addr = INADDR_ANY;

	// build socket 
	SOCKET srv_sock;
	srv_sock=socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == srv_sock) {
		printf("socket create failed.\r\n", WSAGetLastError());
		return false;
	}

	// bind
	err = bind(srv_sock, (struct sockaddr*)&server, sizeof server);
	if (SOCKET_ERROR == err) {
		printf("Udp socket bind error[%d].\r\n", GetLastError());
		return false;
	}
	printf("bind OK\r\n");

	// listen
	if (listen(srv_sock, 10)!=0) {
		printf("listen error\r\n");
		return false;
	}
	printf("listen \r\n");

	SOCKET clt_sock;
	sockaddr_in from;
	int fromlen=sizeof(from);

	while (true) {
		clt_sock=accept(srv_sock, (struct sockaddr*)&from, &fromlen);

		printf("accept one\r\n");

		WSAOVERLAPPED RecvOverlapped = {0};
		// Create an event handle and setup an overlapped structure.
		RecvOverlapped.hEvent = WSACreateEvent();
		if (RecvOverlapped.hEvent == NULL) {
			zr_log_error_ec(WSAGetLastError(), "WSACreateEvent failed");
			return false;
		}

		DWORD RecvBytes = 0, Flags;
		char buf[PKG_LEN];
		WSABUF DataBuf;
		DataBuf.len = PKG_LEN;
		DataBuf.buf = buf;
		int rc;
		while (true) {
			Flags = 0;
			rc= WSARecv(clt_sock, &DataBuf, 1, &RecvBytes, &Flags,
					&RecvOverlapped, NULL);
			if ( (rc == SOCKET_ERROR) && (WSA_IO_PENDING != (err = WSAGetLastError()))) {
				zr_log_error_ec(err, "WSARecv failed");
				break;
			}

			rc = WSAWaitForMultipleEvents(1, &RecvOverlapped.hEvent, TRUE, 
			INFINITE, TRUE);
			if (rc == WSA_WAIT_FAILED) {
				zr_log_error_ec(WSAGetLastError(),
						"WSAWaitForMultipleEvents failed");
				break;
			}
			rc = WSAGetOverlappedResult(clt_sock, &RecvOverlapped, &RecvBytes, 
			FALSE, &Flags);
			if (rc == FALSE) {
				zr_log_error_ec(WSAGetLastError(), "WSARecv operation failed:");
				break;
			}

			// If 0 bytes are received, the connection was closed
			if (RecvBytes == 0)
				break;

			printf("s=%d l=%d\r\n", buf[0], RecvBytes);
			send(clt_sock, buf, RecvBytes, 0);
			printf("send back\r\n");
		}
		printf("connection broken\r\n");
	}
	closesocket(srv_sock);
	return true;
}

bool test_send(char * dest_vip) {
	int err;
//	int local_port = 6112;
//	u_short nport = htons(local_port);
//	char *p_nport = (char *)&nport;

	// sockaddr_in - client
//	struct sockaddr_in client;
//	client.sin_family = AF_INET;
//	client.sin_port = nport;
//	client.sin_addr.s_addr = INADDR_ANY;

	// sockaddr_in - server
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(6112);
	server.sin_addr.s_addr = inet_addr(dest_vip);

	// build socket 
	SOCKET sock;
	sock=socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sock) {
		printf("socket create failed.\r\n", WSAGetLastError());
		return false;
	}

	//	err = bind(sock, (struct sockaddr*)&client, sizeof client);
	//	if (SOCKET_ERROR == err) {
	//		printf("Udp socket bind error[%d].\r\n", GetLastError());
	//		return false;
	//	}

	// set timeout
	int timeout = 3000;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof timeout);

	// data package 
	char buf[PKG_LEN] = { 0 };
	char rbuf[PKG_LEN];

	connect(sock, ( struct sockaddr* )&server, sizeof server);
	printf("connect OK\r\n");

	for (int i=0; i<10; i++) {
		buf[0] = i;

		DWORD tmsend = timeGetTime();
		// send
		printf("sending... ");
		err = send(sock, buf, PKG_LEN, 0);
		DWORD tmsendover = timeGetTime();
		printf(" OK(%d)\r\n", tmsendover - tmsend);

		if (SOCKET_ERROR == err) {
			printf("data  send error.\r\n");
		}

		DWORD RecvBytes = 0, Flags;
		WSAOVERLAPPED RecvOverlapped = {0};

		// Create an event handle and setup an overlapped structure.
		RecvOverlapped.hEvent = WSACreateEvent();
		if (RecvOverlapped.hEvent == NULL) {
			zr_log_error_ec(WSAGetLastError(), "WSACreateEvent failed");
			return false;
		}

		char buf[PKG_LEN];
		WSABUF DataBuf;
		DataBuf.len = PKG_LEN;
		DataBuf.buf = buf;
		while (true) {
			Flags = 0;
			int rc = WSARecv(sock, &DataBuf, 1, &RecvBytes, &Flags,
					&RecvOverlapped, 
					NULL);
			rc = WSAWaitForMultipleEvents(1, &RecvOverlapped.hEvent, TRUE, 
			INFINITE, TRUE);
			if (rc == WSA_WAIT_FAILED) {
				zr_log_error_ec(WSAGetLastError(),
						"WSAWaitForMultipleEvents failed");
				break;
			}
			rc = WSAGetOverlappedResult(sock, &RecvOverlapped, &RecvBytes, 
			FALSE, &Flags);
			if (rc == FALSE) {
				zr_log_error_ec(WSAGetLastError(), "WSARecv failed:");
				break;
			}

			// If 0 bytes are received, the connection was closed
			if (RecvBytes == 0)
				break;

			if (RecvBytes > 0) {
				DWORD tmrecv = timeGetTime();
				printf("s=%d r=%d l=%d\r\n", tmrecv - tmsend, buf[0], RecvBytes);
				break;

			}
		}

	}
	printf("connection is broken\r\n");

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