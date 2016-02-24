//============================================================================
// Name        : dt_ws2_32.cpp
// Author      : zhourui
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <winsock2.h>
#include<map>
#include<string>
#include<iterator>

#include <zr_logger.h>
#include <zr_charset.h>
#include <zr_mem.h>
#include <detours.h>

#include "gf_props.h"
#include "gf_util.h"

using namespace std;

typedef struct conn_info {
	u_short local_nport;
	u_long to_vip;
	u_short to_nport;
} conn_info_t;

map<int, conn_info_t *> conn_info_map;

typedef SOCKET (PASCAL *o_accept)(SOCKET a0, struct sockaddr* a1, int* a2);
typedef int (PASCAL *o_bind)(SOCKET a0, const struct sockaddr* a1, int a2);
typedef int (PASCAL *o_closesocket)(SOCKET a0);
typedef int (PASCAL *o_connect)(SOCKET a0, const struct sockaddr* a1, int a2);
typedef int (PASCAL *o_getpeername)(SOCKET a0, struct sockaddr* a1, int* a2);
typedef int (PASCAL *o_getsockname)(SOCKET a0, struct sockaddr* a1, int* a2);
typedef int (PASCAL *o_getsockopt)(SOCKET a0, int a1, int a2, char* a3, int* a4);
typedef u_long (PASCAL *o_htonl)(u_long a0);
typedef u_short (PASCAL *o_htons)(u_short a0);
typedef int (PASCAL *o_ioctlsocket)(SOCKET a0, long a1, u_long * a2);
typedef unsigned long (PASCAL *o_inet_addr)(const char* a0);
typedef char* (PASCAL *o_inet_ntoa)(struct in_addr a0);
typedef int (PASCAL *o_listen)(SOCKET a0, int a1);
typedef u_long (PASCAL *o_ntohl)(u_long a0);
typedef u_short (PASCAL *o_ntohs)(u_short a0);
typedef int (PASCAL *o_recv)(SOCKET a0, char* a1, int a2, int a3);
typedef int (PASCAL *o_recvfrom)(SOCKET a0, char* a1, int a2, int a3,
		struct sockaddr* a4, int* a5);
typedef int (PASCAL *o_select)(int a0, fd_set* a1, fd_set* a2, fd_set* a3,
		const struct timeval* a4);
typedef int (PASCAL *o_send)(SOCKET a0, const char* a1, int a2, int a3);
typedef int (PASCAL *o_sendto)(SOCKET a0, const char* a1, int a2, int a3,
		const struct sockaddr* a4, int a5);
typedef int (PASCAL *o_setsockopt)(SOCKET a0, int a1, int a2, const char* a3, int a4);
typedef int (PASCAL *o_shutdown)(SOCKET a0, int a1);
typedef SOCKET (PASCAL *o_socket)(int a0, int a1, int a2);
typedef int (PASCAL *o_WSApSetPostRoutine)(int a0);
typedef WSAEVENT (WSAAPI *o_WPUCompleteOverlappedRequest)(SOCKET a0,
		LPWSAOVERLAPPED a1, DWORD a2, DWORD a3, LPINT a4);
typedef SOCKET (WINAPI *o_WSAAccept)(SOCKET a0, struct sockaddr * a1, LPINT a2,
		LPCONDITIONPROC a3, DWORD a4);
typedef INT (WINAPI *o_WSAAddressToStringA)(LPSOCKADDR a0, DWORD a1,
		LPWSAPROTOCOL_INFOA a2, LPSTR a3, LPDWORD a4);
typedef INT (WINAPI *o_WSAAddressToStringW)(LPSOCKADDR a0, DWORD a1,
		LPWSAPROTOCOL_INFOW a2, LPWSTR a3, LPDWORD a4);
typedef BOOL (WINAPI *o_WSACloseEvent)(WSAEVENT a0);
typedef int (WINAPI *o_WSAConnect)(SOCKET a0, const struct sockaddr * a1, int a2,
		LPWSABUF a3, LPWSABUF a4, LPQOS a5, LPQOS a6);
typedef WSAEVENT (WINAPI *o_WSACreateEvent)(void);
typedef int
		(WINAPI *o_WSADuplicateSocketA)(SOCKET a0, DWORD a1, LPWSAPROTOCOL_INFOA a2);
typedef int
		(WINAPI *o_WSADuplicateSocketW)(SOCKET a0, DWORD a1, LPWSAPROTOCOL_INFOW a2);
typedef INT
		(WINAPI *o_WSAEnumNameSpaceProvidersA)(LPDWORD a0, LPWSANAMESPACE_INFOA a1);
typedef INT
		(WINAPI *o_WSAEnumNameSpaceProvidersW)(LPDWORD a0, LPWSANAMESPACE_INFOW a1);
typedef int (WINAPI *o_WSAEnumNetworkEvents)(SOCKET a0, WSAEVENT a1,
		LPWSANETWORKEVENTS a2);
typedef int
		(WINAPI *o_WSAEnumProtocolsA)(LPINT a0, LPWSAPROTOCOL_INFOA a1, LPDWORD a2);
typedef int
		(WINAPI *o_WSAEnumProtocolsW)(LPINT a0, LPWSAPROTOCOL_INFOW a1, LPDWORD a2);
typedef int (WINAPI *o_WSAEventSelect)(SOCKET a0, WSAEVENT a1, long a2);
typedef BOOL (WINAPI *o_WSAGetOverlappedResult)(SOCKET a0, LPWSAOVERLAPPED a1,
		LPDWORD a2, BOOL a3, LPDWORD a4);
typedef BOOL (WINAPI *o_WSAGetQOSByName)(SOCKET a0, LPWSABUF a1, LPQOS a2);
typedef INT (WINAPI *o_WSAGetServiceClassInfoA)(LPGUID a0, LPGUID a1, LPDWORD a2,
		LPWSASERVICECLASSINFOA a3);
typedef INT (WINAPI *o_WSAGetServiceClassInfoW)(LPGUID a0, LPGUID a1, LPDWORD a2,
		LPWSASERVICECLASSINFOW a3);
typedef INT (WINAPI *o_WSAGetServiceClassNameByClassIdA)(LPGUID a0, LPSTR a1,
		LPDWORD a2);
typedef INT (WINAPI *o_WSAGetServiceClassNameByClassIdW)(LPGUID a0, LPWSTR a1,
		LPDWORD a2);
typedef int (WINAPI *o_WSAHtonl)(SOCKET a0, unsigned long a1, unsigned long * a2);
typedef int (WINAPI *o_WSAHtons)(SOCKET a0, unsigned short a1, unsigned short * a2);
typedef INT (WINAPI *o_WSAInstallServiceClassA)(LPWSASERVICECLASSINFOA a0);
typedef INT (WINAPI *o_WSAInstallServiceClassW)(LPWSASERVICECLASSINFOW a0);
typedef int (WINAPI *o_WSAIoctl)(SOCKET a0, DWORD a1, LPVOID a2, DWORD a3, LPVOID a4,
		DWORD a5, LPDWORD a6, LPWSAOVERLAPPED a7,
		LPWSAOVERLAPPED_COMPLETION_ROUTINE a8);
typedef struct hostent* (PASCAL *o_gethostbyaddr)(const char* a0, int a1, int a2);
typedef struct protoent* (PASCAL *o_getprotobyname)(const char* a0);
typedef struct protoent* (PASCAL *o_getprotobynumber)(int a0);
typedef struct servent* (PASCAL *o_getservbyname)(const char* a0, const char* a1);
typedef struct hostent* (PASCAL *o_gethostbyname)(const char* a0);
typedef struct servent* (PASCAL *o_getservbyport)(int a0, const char* a1);
typedef int (PASCAL *o_gethostname)(char* a0, int a1);
typedef SOCKET (WINAPI *o_WSAJoinLeaf)(SOCKET a0, const struct sockaddr * a1, int a2,
		LPWSABUF a3, LPWSABUF a4, LPQOS a5, LPQOS a6, DWORD a7);
typedef int (WINAPI *o_WSANtohl)(SOCKET a0, unsigned long a1, unsigned long * a2);
typedef int (WINAPI *o_WSANtohs)(SOCKET a0, unsigned short a1, unsigned short * a2);
typedef int (WINAPI *o_WSAProviderConfigChange)(LPHANDLE a0, LPWSAOVERLAPPED a1,
		LPWSAOVERLAPPED_COMPLETION_ROUTINE a2);
typedef int (WINAPI *o_WSARecv)(SOCKET a0, LPWSABUF a1, DWORD a2, LPDWORD a3,
		LPDWORD a4, LPWSAOVERLAPPED a5, LPWSAOVERLAPPED_COMPLETION_ROUTINE a6);
typedef int (WINAPI *o_WSARecvDisconnect)(SOCKET a0, LPWSABUF a1);
typedef int (WINAPI *o_WSARecvFrom)(SOCKET a0, LPWSABUF a1, DWORD a2, LPDWORD a3,
		LPDWORD a4, struct sockaddr * a5, LPINT a6, LPWSAOVERLAPPED a7,
		LPWSAOVERLAPPED_COMPLETION_ROUTINE a8);
typedef INT (WINAPI *o_WSARemoveServiceClass)(LPGUID a0);
typedef BOOL (WINAPI *o_WSAResetEvent)(WSAEVENT a0);
typedef int (WINAPI *o_WSASend)(SOCKET a0, LPWSABUF a1, DWORD a2, LPDWORD a3,
		DWORD a4, LPWSAOVERLAPPED a5, LPWSAOVERLAPPED_COMPLETION_ROUTINE a6);
typedef int (WINAPI *o_WSASendDisconnect)(SOCKET a0, LPWSABUF a1);
typedef int (WINAPI *o_WSASendTo)(SOCKET a0, LPWSABUF a1, DWORD a2, LPDWORD a3,
		DWORD a4, const struct sockaddr * a5, int a6, LPWSAOVERLAPPED a7,
		LPWSAOVERLAPPED_COMPLETION_ROUTINE a8);
typedef BOOL (WINAPI *o_WSASetEvent)(WSAEVENT a0);
typedef INT
		(WINAPI *o_WSASetServiceA)(LPWSAQUERYSETA a0, WSAESETSERVICEOP a1, DWORD a2);
typedef INT
		(WINAPI *o_WSASetServiceW)(LPWSAQUERYSETW a0, WSAESETSERVICEOP a1, DWORD a2);
typedef SOCKET (WINAPI *o_WSASocketA)(int a0, int a1, int a2, LPWSAPROTOCOL_INFOA a3,
		GROUP a4, DWORD a5);
typedef SOCKET (WINAPI *o_WSASocketW)(int a0, int a1, int a2, LPWSAPROTOCOL_INFOW a3,
		GROUP a4, DWORD a5);
typedef INT (WINAPI *o_WSAStringToAddressA)(LPSTR a0, INT a1, LPWSAPROTOCOL_INFOA a2,
		LPSOCKADDR a3, LPINT a4);
typedef INT (WINAPI *o_WSAStringToAddressW)(LPWSTR a0, INT a1, LPWSAPROTOCOL_INFOW a2,
		LPSOCKADDR a3, LPINT a4);
typedef DWORD (WINAPI *o_WSAWaitForMultipleEvents)(DWORD a0, const WSAEVENT * a1,
		BOOL a2, DWORD a3, BOOL a4);
typedef int (WINAPI *o_WSCDeinstallProvider)(LPGUID a0, LPINT a1);
typedef int (WINAPI *o_WSCEnableNSProvider)(LPGUID a0, BOOL a1);
typedef int (WINAPI *o_WSCEnumProtocols)(LPINT a0, LPWSAPROTOCOL_INFOW a1, LPDWORD a2,
		LPINT a3);
typedef int (WINAPI *o_WSCGetProviderPath)(LPGUID a0, LPWSTR a1, LPINT a2, LPINT a3);
typedef int (WINAPI *o_WSCInstallNameSpace)(LPWSTR a0, LPWSTR a1, DWORD a2, DWORD a3,
		LPGUID a4);
typedef int (WINAPI *o_WSCInstallProvider)(const LPGUID a0, const LPWSTR a1,
		const LPWSAPROTOCOL_INFOW a2, DWORD a3, LPINT a4);
typedef int (WINAPI *o_WSCUnInstallNameSpace)(LPGUID a0);
typedef int (PASCAL *o_WSAAsyncSelect)(SOCKET a0, HWND a1, u_int a2, long a3);
typedef HANDLE (PASCAL *o_WSAAsyncGetProtoByNumber)(HWND a0, u_int a1, int a2,
		char* a3, int a4);
typedef HANDLE (PASCAL *o_WSAAsyncGetProtoByName)(HWND a0, u_int a1, const char* a2,
		char* a3, int a4);
typedef HANDLE (PASCAL *o_WSAAsyncGetServByPort)(HWND a0, u_int a1, int a2,
		const char* a3, char* a4, int a5);
typedef HANDLE (PASCAL *o_WSAAsyncGetServByName)(HWND a0, u_int a1, const char* a2,
		const char* a3, char* a4, int a5);
typedef int (PASCAL *o_WSACancelAsyncRequest)(HANDLE a0);
typedef FARPROC (PASCAL *o_WSASetBlockingHook)(FARPROC a0);
typedef int (PASCAL *o_WSAUnhookBlockingHook)(void);
typedef int (PASCAL *o_WSAGetLastError)(void);
typedef void (PASCAL *o_WSASetLastError)(int a0);
typedef int (PASCAL *o_WSACancelBlockingCall)(void);
typedef BOOL (PASCAL *o_WSAIsBlocking)(void);
typedef int (PASCAL *o_WSAStartup)(WORD a0, LPWSADATA a1);
typedef int (PASCAL *o_WSACleanup)(void);
typedef int (PASCAL *o___WSAFDIsSet)(SOCKET a0, fd_set* a1);
typedef int (PASCAL *o_WEP)(void);
typedef int (PASCAL *o_WSCWriteNameSpaceOrder)(int a0, int a1);
typedef int (PASCAL *o_WSCWriteProviderOrder)(LPDWORD a0, DWORD a1);
typedef int (PASCAL *o_WSANSPIoctl)(HANDLE a0, DWORD a1, LPVOID a2, DWORD a3,
		LPVOID a4, DWORD a5, LPDWORD a6, LPVOID a7);
typedef int (PASCAL *o_WSCUpdateProvider)(LPGUID a0, const WCHAR FAR* a1,
		const LPVOID a2, DWORD a3, LPINT a4);
typedef int (PASCAL *o_getaddrinfo)(const char* a0, const char* a1, LPVOID a2,
		LPVOID a3);
typedef void (PASCAL *o_freeaddrinfo)(LPVOID a0);
typedef int (PASCAL *o_getnameinfo)(LPVOID a0, DWORD a1, char* a2, DWORD a3, char* a4,
		DWORD a5, int a6);
typedef HANDLE (WINAPI *o_WSAAsyncGetHostByAddr)(HWND a0, u_int a1, const char* a2,
		int a3, int a4, char* a5, int a6);
typedef HANDLE (WINAPI *o_WSAAsyncGetHostByName)(HWND a0, u_int a1, const char* a2,
		char* a3, int a4);
typedef INT
		(WINAPI *o_WSALookupServiceBeginA)(LPWSAQUERYSETA a0, DWORD a1, LPHANDLE a2);
typedef INT
		(WINAPI *o_WSALookupServiceBeginW)(LPWSAQUERYSETW a0, DWORD a1, LPHANDLE a2);
typedef INT (WINAPI *o_WSALookupServiceEnd)(HANDLE a0);
typedef INT (WINAPI *o_WSALookupServiceNextA)(HANDLE a0, DWORD a1, LPDWORD a2,
		LPWSAQUERYSETA a3);
typedef INT (WINAPI *o_WSALookupServiceNextW)(HANDLE a0, DWORD a1, LPDWORD a2,
		LPWSAQUERYSETW a3);

o_recvfrom p_o_recvfrom;
o_send p_o_send;
o_sendto p_o_sendto;
o_connect p_o_connect;
o_gethostbyname p_o_gethostbyname;
o_WSARecv p_o_WSARecv;
o_WSARecvFrom p_o_WSARecvFrom;
o_closesocket p_o_closesocket;
//o_recv p_o_recv;
// old proc define
//o_accept p_o_accept;
//o_bind p_o_bind;
//o_getpeername p_o_getpeername;
//o_getsockname p_o_getsockname;
//o_getsockopt p_o_getsockopt;
//o_htonl p_o_htonl;
//o_htons p_o_htons;
//o_ioctlsocket p_o_ioctlsocket;
//o_inet_addr p_o_inet_addr;
//o_inet_ntoa p_o_inet_ntoa;
//o_listen p_o_listen;
//o_ntohl p_o_ntohl;
//o_ntohs p_o_ntohs;
//o_select p_o_select;
//o_setsockopt p_o_setsockopt;
//o_shutdown p_o_shutdown;
//o_socket p_o_socket;
//o_WSApSetPostRoutine p_o_WSApSetPostRoutine;
//o_WPUCompleteOverlappedRequest p_o_WPUCompleteOverlappedRequest;
//o_WSAAccept p_o_WSAAccept;
//o_WSAAddressToStringA p_o_WSAAddressToStringA;
//o_WSAAddressToStringW p_o_WSAAddressToStringW;
//o_WSACloseEvent p_o_WSACloseEvent;
//o_WSAConnect p_o_WSAConnect;
//o_WSACreateEvent p_o_WSACreateEvent;
//o_WSADuplicateSocketA p_o_WSADuplicateSocketA;
//o_WSADuplicateSocketW p_o_WSADuplicateSocketW;
//o_WSAEnumNameSpaceProvidersA p_o_WSAEnumNameSpaceProvidersA;
//o_WSAEnumNameSpaceProvidersW p_o_WSAEnumNameSpaceProvidersW;
//o_WSAEnumNetworkEvents p_o_WSAEnumNetworkEvents;
//o_WSAEnumProtocolsA p_o_WSAEnumProtocolsA;
//o_WSAEnumProtocolsW p_o_WSAEnumProtocolsW;
//o_WSAEventSelect p_o_WSAEventSelect;
//o_WSAGetOverlappedResult p_o_WSAGetOverlappedResult;
//o_WSAGetQOSByName p_o_WSAGetQOSByName;
//o_WSAGetServiceClassInfoA p_o_WSAGetServiceClassInfoA;
//o_WSAGetServiceClassInfoW p_o_WSAGetServiceClassInfoW;
//o_WSAGetServiceClassNameByClassIdA p_o_WSAGetServiceClassNameByClassIdA;
//o_WSAGetServiceClassNameByClassIdW p_o_WSAGetServiceClassNameByClassIdW;
//o_WSAHtonl p_o_WSAHtonl;
//o_WSAHtons p_o_WSAHtons;
//o_WSAInstallServiceClassA p_o_WSAInstallServiceClassA;
//o_WSAInstallServiceClassW p_o_WSAInstallServiceClassW;
//o_WSAIoctl p_o_WSAIoctl;
//o_gethostbyaddr p_o_gethostbyaddr;
//o_getprotobyname p_o_getprotobyname;
//o_getprotobynumber p_o_getprotobynumber;
//o_getservbyname p_o_getservbyname;
//o_getservbyport p_o_getservbyport;
//o_gethostname p_o_gethostname;
//o_WSAJoinLeaf p_o_WSAJoinLeaf;
//o_WSANtohl p_o_WSANtohl;
//o_WSANtohs p_o_WSANtohs;
//o_WSAProviderConfigChange p_o_WSAProviderConfigChange;
//o_WSARecvDisconnect p_o_WSARecvDisconnect;
//o_WSARemoveServiceClass p_o_WSARemoveServiceClass;
//o_WSAResetEvent p_o_WSAResetEvent;
//o_WSASend p_o_WSASend;
//o_WSASendDisconnect p_o_WSASendDisconnect;
//o_WSASendTo p_o_WSASendTo;
//o_WSASetEvent p_o_WSASetEvent;
//o_WSASetServiceA p_o_WSASetServiceA;
//o_WSASetServiceW p_o_WSASetServiceW;
//o_WSASocketA p_o_WSASocketA;
//o_WSASocketW p_o_WSASocketW;
//o_WSAStringToAddressA p_o_WSAStringToAddressA;
//o_WSAStringToAddressW p_o_WSAStringToAddressW;
//o_WSAWaitForMultipleEvents p_o_WSAWaitForMultipleEvents;
//o_WSCDeinstallProvider p_o_WSCDeinstallProvider;
//o_WSCEnableNSProvider p_o_WSCEnableNSProvider;
//o_WSCEnumProtocols p_o_WSCEnumProtocols;
//o_WSCGetProviderPath p_o_WSCGetProviderPath;
//o_WSCInstallNameSpace p_o_WSCInstallNameSpace;
//o_WSCInstallProvider p_o_WSCInstallProvider;
//o_WSCUnInstallNameSpace p_o_WSCUnInstallNameSpace;
//o_WSAAsyncSelect p_o_WSAAsyncSelect;
//o_WSAAsyncGetProtoByNumber p_o_WSAAsyncGetProtoByNumber;
//o_WSAAsyncGetProtoByName p_o_WSAAsyncGetProtoByName;
//o_WSAAsyncGetServByPort p_o_WSAAsyncGetServByPort;
//o_WSAAsyncGetServByName p_o_WSAAsyncGetServByName;
//o_WSACancelAsyncRequest p_o_WSACancelAsyncRequest;
//o_WSASetBlockingHook p_o_WSASetBlockingHook;
//o_WSAUnhookBlockingHook p_o_WSAUnhookBlockingHook;
//o_WSAGetLastError p_o_WSAGetLastError;
//o_WSASetLastError p_o_WSASetLastError;
//o_WSACancelBlockingCall p_o_WSACancelBlockingCall;
//o_WSAIsBlocking p_o_WSAIsBlocking;
//o_WSAStartup p_o_WSAStartup;
//o_WSACleanup p_o_WSACleanup;
//o___WSAFDIsSet p_o___WSAFDIsSet;
//o_WEP p_o_WEP;
//o_WSCWriteNameSpaceOrder p_o_WSCWriteNameSpaceOrder;
//o_WSCWriteProviderOrder p_o_WSCWriteProviderOrder;
//o_WSANSPIoctl p_o_WSANSPIoctl;
//o_WSCUpdateProvider p_o_WSCUpdateProvider;
//o_getaddrinfo p_o_getaddrinfo;
//o_freeaddrinfo p_o_freeaddrinfo;
//o_getnameinfo p_o_getnameinfo;
//o_WSAAsyncGetHostByAddr p_o_WSAAsyncGetHostByAddr;
//o_WSAAsyncGetHostByName p_o_WSAAsyncGetHostByName;
//o_WSALookupServiceBeginA p_o_WSALookupServiceBeginA;
//o_WSALookupServiceBeginW p_o_WSALookupServiceBeginW;
//o_WSALookupServiceEnd p_o_WSALookupServiceEnd;
//o_WSALookupServiceNextA p_o_WSALookupServiceNextA;
//o_WSALookupServiceNextW p_o_WSALookupServiceNextW;

#define MAX_PKT_LEN	4096
HMODULE hdl= NULL;

char processname[MAX_PATH];
gf_props_t props;
char nickname[16];

int isFirstConnect_tcp= FALSE;
u_long to_vip_tcp;
u_short to_nport_tcp;
bool stopWar3ChangeNameThread = true;

void init_o_procs();

char newname[16];

char __youarehaker[128];

void __declspec(dllexport) PASCAL urh() {
	char a='y';
	char b='o';
	char c='u';
	char d=' ';
	char e='a';
	char f='r';
	char g='e';
	char h=' ';
	char i='h';
	char j='a';
	char k='c';
	char l='k';
	char m='e';
	char n='r';
	char o='.';
	char p=' ';

	__youarehaker[0] = '\0';
	strcpy(__youarehaker, "You are hacker. :)");

	//	return __youarehaker;

}

void __declspec(dllexport) PASCAL ggame1() {
}
void __declspec(dllexport) PASCAL ggame2() {
}
void __declspec(dllexport) PASCAL ggame3() {
}
void __declspec(dllexport) PASCAL ggame4() {
}
void __declspec(dllexport) PASCAL ggame5() {
}
void __declspec(dllexport) PASCAL ggame6() {
}
void __declspec(dllexport) PASCAL ggame7() {
}
void __declspec(dllexport) PASCAL ggame8() {
}
void __declspec(dllexport) PASCAL ggame9() {
}
void __declspec(dllexport) PASCAL ggame0() {
}

SOCKET war3ClientFightSocket = 0;
int PASCAL my_closesocket(SOCKET s) {
	//判断是否是war3 客户端 战斗用的Socket
	if (war3ClientFightSocket == s) {
		war3ClientFightSocket = 0;
		saveWar3Replay();
	}
	return (p_o_closesocket)(s);
}
//
//int PASCAL my_WSAAccept(SOCKET s, struct sockaddr * p_addr, LPINT p_addrlen,
//		LPCONDITIONPROC lpfnCondition, DWORD dwCallbackData){
//	zr_log_debug_ec(0, "WSAAccept on connect");
//	return (p_o_WSAAccept)(s, p_addr, p_addrlen, lpfnCondition,dwCallbackData);
//}

int PASCAL my_connect(SOCKET s, const struct sockaddr* name, int namelen) {
	//	zr_log_debug("connect()");

	// get socket type - udp or tcp
	int sock_type = 0;
	int optlen = 4;
	getsockopt(s, SOL_SOCKET, SO_TYPE, (char*)&sock_type, &optlen);

	sockaddr_in sin;
	sockaddr_in * psin = &sin;
	memcpy(psin, name, sizeof(struct sockaddr));

	// 判断Connect的目标ip是否是属于虚拟网IP
	if (memcmp(&(psin->sin_addr), &(props.v_ipaddr), props.v_netmask_c) == 0) {
		zr_log_debug_ec(
				0,
				"connect %d.%d.%d.%d is a vip, the connection switch to pipe host %s:%d.",
				psin->sin_addr.S_un.S_un_b.s_b1,
				psin->sin_addr.S_un.S_un_b.s_b2,
				psin->sin_addr.S_un.S_un_b.s_b3,
				psin->sin_addr.S_un.S_un_b.s_b4, //
				props.pipe_host, //
				props.pipe_tcpport);

		//保留好目标IP和Port
		u_long to_vip = psin->sin_addr.S_un.S_addr;
		u_long to_nport = psin->sin_port;

		// 将真正的连接IP指向Pipe Host的IP
		psin->sin_addr.S_un.S_addr = inet_addr(props.pipe_host);
		if (SOCK_STREAM == sock_type) {
			//记录要connect的虚拟地址,到全局变量，见send()
			to_vip_tcp = to_vip;
			to_nport_tcp = to_nport;

			// 将真正的连接Port指向Pipe Host的Tcp Port
			psin->sin_port = htons(props.pipe_tcpport);

			//标记TCP的连接刚刚创建，注：对Send的第一个包有影响。
			isFirstConnect_tcp = true;

			//war3GamerNameAddr = getWar3GamernameAddr();
			//TODO: 改游戏玩家名，注意：不一定成功
			//changeWar3GamerName();
			//stopWar3ChangeNameThread = true;

			//清除war3客户端实际战斗的TCP Socket 数值
			war3ClientFightSocket = 0;

			zr_log_debug("connect with TCP");

			stopWar3ChangeNameThread = true;
		} else if (SOCK_DGRAM == sock_type) {
			//从Socket参数中通过getsockname()获得本地的端口
			sockaddr_in src_sin;
			int ss_len = sizeof(src_sin);
			getsockname(s, (SOCKADDR*)&src_sin, &ss_len);
			conn_info_t * p_ci = (conn_info_t *)malloc(sizeof conn_info_t);

			//本地端口记入conn_info_map
			p_ci->local_nport = src_sin.sin_port;
			p_ci->to_vip = to_vip;
			p_ci->to_nport = to_nport;
			conn_info_map[p_ci->local_nport] = p_ci;

			// 将真正的连接Port指向Pipe Host的Udp Port
			psin->sin_port = htons(props.pipe_udpport);

			zr_log_debug("connect with UDP");
		} else {
			zr_log_debug("connect with ........uh....I don't know.");
		}

	} else {
		zr_log_debug_ec( 0, "connect to %d.%d.%d.%d",
				psin->sin_addr.S_un.S_un_b.s_b1,
				psin->sin_addr.S_un.S_un_b.s_b2,
				psin->sin_addr.S_un.S_un_b.s_b3,
				psin->sin_addr.S_un.S_un_b.s_b4);
	}

	return (p_o_connect)(s, ( struct sockaddr* )psin, namelen);
}

int PASCAL my_recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from,
		int* fromlen) {
	char * p = (char *)malloc(len + 16);
	int iRet = (p_o_recvfrom)(s, p, len + 16, flags, from, fromlen);
	//	zr_log_debug_ec(iRet, "recvfrom()");
	if (iRet == SOCKET_ERROR)
		return iRet;

	// Nat Keeping Packege
	if (1 == iRet && 255 == p[0]) {
		free(p);
		return iRet;
	}

	struct sockaddr_in * psin = (struct sockaddr_in *)from;
	if ((p[4] == 0xff && p[5] == 0xff && p[6] == 0xff && p[7] == 0xff)
			|| memcmp(p + 4, &(props.v_ipaddr), props.v_netmask_c) == 0) {
		//更改来源地址
		psin->sin_addr.S_un.S_addr = *(u_long *)p;

		//更改来源端口
		u_char * pp = (u_char *)(&(psin->sin_port));
		pp[0]= p[8];
		pp[1] =p[9];

		//注意指针+16
		memcpy(buf, p + 16, len);
		free(p);
		iRet -= 16;
	} else {
		memcpy(buf, p, len);
		free(p);
	}

	return iRet;
}

void setNoDelay(SOCKET s, int isNoDelay) {
	BOOL oldNoDelay;
	int err = 0;
	int optlen = sizeof oldNoDelay;
	err = getsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char *)&oldNoDelay, &optlen);
	if (SOCKET_ERROR == err) {
		zr_log_error_ec(WSAGetLastError(),
				"getsockopt TCP_NODELAY failed. err=%d\r\n", err);
		return;
	}
	if (oldNoDelay == isNoDelay)
		return;

	BOOL newNoDelay = isNoDelay;
	err = setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char *)&newNoDelay,
			sizeof newNoDelay);
	if (SOCKET_ERROR == err) {
		zr_log_error_ec(WSAGetLastError(),
				"setsockopt TCP_NODELAY failed. err=%d\r\n", err);
		return;
	}
	 getsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char *)&newNoDelay, &optlen);
	 zr_log_debug_ec(0, "change TCP_NODELAY %d=>%d\r\n",oldNoDelay, newNoDelay);
	 
}

int PASCAL my_send(SOCKET s, const char* buf, int len, int flags) {
	//	zr_log_debug_ec(0, "send(), len=%d", len);

	// get socket type - udp or tcp
	int sock_type = 0;
	int optlen = 4;
	getsockopt(s, SOL_SOCKET, SO_TYPE, (char*)&sock_type, &optlen);

	if (SOCK_STREAM == sock_type) { // TCP
		//		zr_log_debug("send with TCP");
		
		if (isFirstConnect_tcp) {
			zr_log_debug("isFirstConnect_tcp=true");
			setNoDelay(s, 1);
			/**
			 * TCP Connect 后第一个数据包需加上"头部数据",格式如下：
			 * 1 字节：0x12表示自己的身份是client
			 * 32 字节：用户名
			 * 4 字节：安全Key
			 * 4 字节：自己的虚拟IP地址
			 * 4 字节：目标主机的虚拟IP地址
			 * 2 字节：目标主机的虚拟端口号
			 * 之后  ：原本要发送的数据
			 */
			char new_buf[MAX_PKT_LEN] = "\0";
			int new_buf_len = 0;
			int off = 0;

			new_buf[off] = 0x12;
			off ++;

			strcpy(new_buf + off, props.username);
			off +=32;

			strcpy(new_buf + off, "skey");
			off += 4;

			memcpy(new_buf + off, &(props.v_ipaddr), 4);
			off += 4;

			memcpy(new_buf + off, &to_vip_tcp, 4);
			off += 4;

			u_char * pp = (u_char *)&(to_nport_tcp);
			new_buf[off] = pp[0];
			off ++;
			new_buf[off] = pp[1];
			off ++;

#if 1
			//检查0x1e标记[进主], NOTE: 通常在war3里，不出意外的话，这个数值肯定是0x1e
			u_char tag = buf[1];
			if (tag == 0x1e) {
				//changeWar3GamerName();

				// 改包
				int bufNameBeginPos = 19;
				int bufNameEndNextPos = 19;
				for (int i=19; i<100; i++) {
					if (buf[i] == 0) {
						bufNameEndNextPos = i + 1;
						break;
					}
				}

				// 改游戏用户名
				int newnameBufLen = strlen(newname) + 1;

				//添加数据

				// copy 名称前面的内容
				memcpy(new_buf + off, buf, bufNameBeginPos);
				off += bufNameBeginPos;

				// copy 名称
				memcpy(new_buf + off, newname, newnameBufLen);
				off += newnameBufLen;

				// copy 名称前面的内容
				memcpy(new_buf + off, buf + bufNameEndNextPos, len
						- bufNameEndNextPos);
				off += len - bufNameEndNextPos;

				// new_buf 大小
				new_buf_len = off;

				// 原包的2-3字节是short类型，原包长度。改成新的
				// (对应新包49-50)
				u_short * p_oldPkgLen;
				p_oldPkgLen = (u_short *)(new_buf + 49);
				*p_oldPkgLen = new_buf_len - 47;
			} else {
				//添加数据
				memcpy(new_buf + off, buf, len);
				new_buf_len = off + len;
			}
#endif

#if 0
			memcpy(new_buf + off, buf, len);
			new_buf_len = off + len;
#endif

			/* 包装成新的WSABUF*/
			isFirstConnect_tcp = false;
			return (p_o_send)(s, new_buf, new_buf_len, flags);
		} else { //不是第一组包
			if (0xf7 == (byte)buf[0]) { // is War3 
				switch ((byte)buf[1]) {
				case 0x0b: { //主机？战斗开始
					setNoDelay(s, 1);
					stopWar3ChangeNameThread = true;
					zr_log_debug_ec(0, "Fight begin. cname thread pause");
					break;
				}
					//				case 0x1B: {//战斗结束
					//					stopWar3ChangeNameThread = false;
					//					zr_log_debug_ec(0, "Fight end. cname thread continue");
					//					break;
					//				}
				case 0x23: {//客户机开始游戏读图??
					stopWar3ChangeNameThread = true;
					zr_log_debug_ec(0, "Map read begin. cname thread pause");
					war3ClientFightSocket = s;
					break;
				}
					//				case 0x21: { //客户机主动退出游戏
					//					stopWar3ChangeNameThread = false;
					//					zr_log_debug_ec(0, "UserQuit. cname thread continue");
					//					break;
					//				}
					//				case 0x14: {
					//					zr_log_debug("*****************Game OVER 14*********************");
					//					break;
					//				}
					//				case 0x07: {
					//					zr_log_debug("*****************Game OVER 07*********************");
					//					break;
					//				}
					//				case 0x1B: {
					//					zr_log_debug("*****************Game OVER 1B*********************");
					//					break;
					//				}
				default: {

				}
				}
			}
		}
	} else if (SOCK_DGRAM == sock_type) { // UDP
		//		zr_log_debug("send with UDP");

		char new_buf[MAX_PKT_LEN] = "\0";
		/**
		 * UDP数据包全部附加一个启始头部数据，格式如下：
		 * 4 字节：自己的 虚拟IP地址
		 * 4 字节：目标的 虚拟IP地址
		 * 2 字节：自己的 端口号
		 * 2 字节：目标的 端口号
		 * 4 字节：（保留，未使用）
		 * 从第16字节开始：原本要发送的数据
		 */
		memcpy(new_buf, &(props.v_ipaddr), 4);

		//获取Socket参数的本地Port
		sockaddr_in src_sin;
		int sa_len = sizeof(src_sin);
		getsockname(s, (SOCKADDR*)&src_sin, &sa_len);
		int nport = 0;

		//从conn_info_map 通过本地Port作为Key,取得conn_info
		if (conn_info_map.count(src_sin.sin_port)) {
			conn_info_t * p_ci = conn_info_map[src_sin.sin_port];
			//将目标虚拟IP地址写入udp数据头
			memcpy(new_buf + 4, &(p_ci->to_vip), 4);
			//记下虚拟IP地址的目标端口
			nport = p_ci->to_nport;
			//TODO: when to release the memory of p_ci??
		} else {
			zr_log_debug("NO found conn info!!!! Maybe a error!!");
		}
		//		char sock_key[64];
		//		springf(sock_key, "%i.%i,%i,%i:%d", 
		//				dest_sin.sin_addr.S_un.S_un_b.s_b1,
		//				dest_sin.sin_addr.S_un.S_un_b.s_b2,
		//				dest_sin.sin_addr.S_un.S_un_b.s_b3,
		//				dest_sin.sin_addr.S_un.S_un_b.s_b4,
		//				ntohs(dest_sin.sin_port)

		//封装源端口
		u_char * pp = (u_char *)&(src_sin.sin_port);
		new_buf[8] = pp[0];
		new_buf[9] = pp[1];

		//封装目标端口
		pp = (u_char *)&(nport);
		new_buf[10] = pp[0];
		new_buf[11] = pp[1];

		//封装数据
		memcpy(new_buf + 16, buf, len);
		return (p_o_send)(s, new_buf, len + 16, flags);
	}

	return (p_o_send)(s, buf, len, flags);
}

//int PASCAL my_recv( SOCKET s, char *buf,int len,int flags) {
//	zr_log_debug_ec(0, "my_recv()");
//	int iRet = (p_o_recv)(s, buf, len, flags);
//
//	int sock_type = 0;
//	int optlen = 4;
//	getsockopt(s, SOL_SOCKET, SO_TYPE, (char*)&sock_type, &optlen);
//
//	if (iRet == SOCKET_ERROR)
//		return iRet;
//
//	if (SOCK_DGRAM == sock_type) {
//		return iRet;
//	}else if(SOCK_STREAM == sock_type){
//		zr_log_debug("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&77");
//		return iRet;
//	}
//	return iRet;
//}


void changeAUCPlayerNameAddr(PVOID beginAddr) {
	return;
	// AUCPlayer...
	byte key[] = { (byte)0x41, (byte)0x55, (byte)0x43, (byte)0x50, (byte)0x6C,
			(byte)0x61, (byte)0x79, (byte)0x65, (byte)0x72, (byte)0x40,
			(byte)0x43, (byte)0x4E, (byte)0x65, (byte)0x74, (byte)0x44,
			(byte)0x61, (byte)0x74, (byte)0x61, (byte)0x40, (byte)0x40,
			(byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00,
			(byte)0x00, (byte)0x50 };
	PVOID res = zr_memscan_baseaddr(GetCurrentProcess(), beginAddr, 0x60000000,
			key, sizeof key);
	if (res) {
		char safeName[16];
		memset(safeName, 0, 16);
		//strncpy(safeName, newname, 15);
		SIZE_T rlen = 0;
		ReadProcessMemory(GetCurrentProcess(), (PBYTE)res, safeName, 16, &rlen);
		zr_log_debug_ec(GetLastError(), "-------------- %d %d", safeName[0],
				rlen);
		if (!WriteProcessMemory(GetCurrentProcess(), (PBYTE)res + 0xAA,
				safeName, 16, &rlen)) {
			zr_log_debug_ec(GetLastError(), "WriteProcessMemory error");
		}

	}
}
void changeHostGamerNameAddr() {
	byte key[] = { (byte)0x45, (byte)0x3A, (byte)0x5C, (byte)0x44, (byte)0x72,
			(byte)0x69, (byte)0x76, (byte)0x65, (byte)0x31, (byte)0x5C,
			(byte)0x74, (byte)0x65, (byte)0x6D, (byte)0x70, (byte)0x5C,
			(byte)0x62, (byte)0x75, (byte)0x69, (byte)0x6C, (byte)0x64,
			(byte)0x77, (byte)0x61, (byte)0x72, (byte)0x33, (byte)0x78,
			(byte)0x5C, (byte)0x45, (byte)0x6E, (byte)0x67, (byte)0x69,
			(byte)0x6E, (byte)0x65, (byte)0x5C, (byte)0x53, (byte)0x6F,
			(byte)0x75, (byte)0x72, (byte)0x63, (byte)0x65, (byte)0x5C,
			(byte)0x4E, (byte)0x65, (byte)0x74, (byte)0x5C, (byte)0x4E,
			(byte)0x65, (byte)0x74, (byte)0x52, (byte)0x6F, (byte)0x75,
			(byte)0x74, (byte)0x65, (byte)0x72, (byte)0x2E, (byte)0x63,
			(byte)0x70, (byte)0x70, (byte)0x00, (byte)0x00, (byte)0x00,
			(byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00,
			(byte)0x00, (byte)0x00, (byte)0x00, (byte)0xE8, (byte)0x00 };

	PVOID res = zr_memscan_baseaddr(GetCurrentProcess(), (PBYTE)0x5000000,
			0x60000000, key, sizeof key);
	if (res) {
		char safeName[16];
		memset(safeName, 0, 16);
		strncpy(safeName, newname, 15);
		SIZE_T rlen = 0;
		WriteProcessMemory(GetCurrentProcess(), (PBYTE)res + 0x16D, safeName,
				16, &rlen);
	}

	changeAUCPlayerNameAddr(res);
}

PVOID hostGamerNameAddr = 0;
// used by war3 udp
int PASCAL my_sendto(SOCKET s, const char* buf, int len, int flags,
		const struct sockaddr* to, int tolen) {
	struct sockaddr_in * psin =( struct sockaddr_in *)to;
	char new_buf[MAX_PKT_LEN] = "\0";
	/**
	 * UDP数据包全部附加一个启始头部数据，格式如下：
	 * 4 字节：自己的 虚拟IP地址
	 * 4 字节：目标的 虚拟IP地址
	 * 2 字节：自己的 端口号
	 * 2 字节：目标的 端口号
	 * 4字节：（保留，未使用）
	 * 从第16字节开始：原本要发送的数据
	 */
	memcpy(new_buf, &(props.v_ipaddr), 4);
	memcpy(new_buf + 4, &(psin->sin_addr), 4);

	sockaddr_in src_sin;
	int sa_len = sizeof(src_sin);
	getsockname(s, (SOCKADDR*)&src_sin, &sa_len);
	u_char * pp = (u_char *)&(src_sin.sin_port);
	new_buf[8] = pp[0];
	new_buf[9] = pp[1];

	pp = (u_char *)&(psin->sin_port);
	new_buf[10] = pp[0];
	new_buf[11] = pp[1];

	bool isDataCopied = false;
	switch ((u_char)buf[0]) {
	case 0xf7: {
		switch ((u_char)buf[1]) {
		case 0x30: {// war3 data 更改主机名“局域网内的游戏” // 特别注意：props.username不能大于32字符
			char new_name[32];
			ZeroMemory(new_name, 32);
			char safename[21];
			ZeroMemory(safename, 21);

			strncpy(safename, newname, 20);
			char speedstr[32];
			if (props.myspeed >= 1000 || props.myspeed < 0)
				sprintf(speedstr, ">1000");
			else
				sprintf(speedstr, "%d", props.myspeed);
			sprintf(new_name, "%s %sms ", safename, speedstr);
			//strncpy(new_name, props.username, 31);
			int new_name_len = strlen(new_name);

			char * old_name = (char *) buf + 20;
			int old_name_len = strlen(old_name);
			char * next_pos = (char *)buf + 20 + old_name_len + 1;

			char * mhost_buf = new_buf + 16;
			memcpy(mhost_buf, buf, 20);
			memcpy(mhost_buf + 20, new_name, new_name_len + 1);
			memcpy(mhost_buf + 20 + new_name_len + 1, next_pos, len - (next_pos
					- buf));
			char * _tmp_p = mhost_buf;
			short mhost_buf_len = len + new_name_len - old_name_len;
			short * p_mhost_buf_len = &mhost_buf_len;
			mhost_buf[2] = *((char *)p_mhost_buf_len);
			mhost_buf[3] = *((char *)(p_mhost_buf_len) + 1);
			len = mhost_buf_len;
			isDataCopied = true;

			break;
		}
		case 0x31: {
			if (hostGamerNameAddr <= 0) {
				DWORD far threadID;
				//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)changeHostGamerNameAddr, NULL, 0, &threadID);
			}
			break;
		}
		case 0x32: {
			//			zr_log_debug("I created a war3 host.");
			break;
		}
		case 0x33: {
			//			zr_log_debug("My war3 host has began or been canceled.");
			break;
		}
		case 0x2f: {//回应主机，我是个war3
			stopWar3ChangeNameThread = false;
			//zr_log_debug_ec(0, "Client waiting host. cname thread continue");
			break;
		}
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
	if (!isDataCopied)
		memcpy(new_buf + 16, buf, len);

	/**
	 * 将UDP发送目标转向pipe_host:pipe_udpport
	 */
	sockaddr_in new_sin;
	new_sin.sin_family = AF_INET;
	new_sin.sin_addr.S_un.S_addr = inet_addr(props.pipe_host);
	new_sin.sin_port = htons(props.pipe_udpport);

	int new_len = len + 16;

	return (p_o_sendto)(s, new_buf, new_len, flags, (struct sockaddr*)&new_sin,
			tolen);
}

struct hostent* PASCAL my_gethostbyname(const char* name) {
	//	zr_log_debug_ec(0, "gethostbyname %s", name);
	hostent* p_hostent = (p_o_gethostbyname)(name);
	if (NULL == p_hostent)
		return p_hostent;
	if (NULL == p_hostent->h_addr_list[0])
		return p_hostent;
	memcpy(p_hostent->h_addr_list[0], &(props.v_ipaddr),
			sizeof (props.v_ipaddr));
	return p_hostent;
}

int WINAPI my_WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
		LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags,
		LPWSAOVERLAPPED lpOverlapped,
		LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	//zr_log_debug("WSARecv()");
	//setNoDelay(s, 1);
	
	int iRet = (p_o_WSARecv)(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd,
			lpFlags, lpOverlapped, lpCompletionRoutine);
	if (iRet == SOCKET_ERROR ) {
			return iRet;
	}
	int sock_type = 0;
	int optlen = sizeof sock_type;
	getsockopt(s, SOL_SOCKET, SO_TYPE, (char*)&sock_type, &optlen);
	if (SOCK_DGRAM == sock_type) {
		//	struct sockaddr_in * psin = (struct sockaddr_in *)lpFrom;
		u_char * p = (u_char *)lpBuffers[0].buf;

		// Nat Keeping Packege
		if (1 == *lpNumberOfBytesRecvd && 255 == p[0]) {
			return iRet;
		}

		if ((p[4] == 0xff && p[5] == 0xff && p[6] == 0xff && p[7] == 0xff)
				|| memcmp(p + 4, &(props.v_ipaddr), props.v_netmask_c) == 0) {
			//更改来源地址
			//		psin->sin_addr.S_un.S_addr = *(u_long *)p;

			//更改来源端口
			//		u_char * pp = (u_char *)(&(psin->sin_port));
			//		pp[0]= p[8];
			//		pp[1] =p[9];

			//更改数据长度
			*lpNumberOfBytesRecvd -= 16;

			//更改buf长度的标示值
			lpBuffers[0].len = lpBuffers[0].len - 16;

			//注意指针+16
			memcpy(lpBuffers[0].buf, lpBuffers[0].buf + 16,
					*lpNumberOfBytesRecvd);
		}
	} else if (SOCK_STREAM == sock_type) {
		//zr_log_debug_ec(WSAGetLastError(), "WSARecv() TCP");
		//setNoDelay(s, 1);
		//		zr_log_debug_ec(0, "%d", lpBuffers[0].buf[0]);
		//		if (0xf7 == (byte)lpBuffers[0].buf[0]) { // is War3 
		//			zr_log_debug_ec(0, "IS WAR 3");
		//			switch ((byte)lpBuffers[0].buf[1]) {
		//			}
		//		}
	}

	return iRet;
}

// used by war3 udp
int WINAPI my_WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
		LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags,
		struct sockaddr * lpFrom, LPINT lpFromlen,
		LPWSAOVERLAPPED lpOverlapped,
		LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	int iRet = (p_o_WSARecvFrom)(s, lpBuffers, dwBufferCount,
			lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped,
			lpCompletionRoutine);
	//zr_log_debug_ec(iRet, "my_WSARecvFrom()");
	if (iRet == SOCKET_ERROR)
		return iRet;

	struct sockaddr_in * psin = (struct sockaddr_in *)lpFrom;
	u_char * p = (u_char *)lpBuffers[0].buf;

	// Nat Keeping Packege
	if (1 == *lpNumberOfBytesRecvd && 255 == p[0]) {
		return iRet;
	}

	if ((p[4] == 0xff && p[5] == 0xff && p[6] == 0xff && p[7] == 0xff)
			|| memcmp(p + 4, &(props.v_ipaddr), props.v_netmask_c) == 0) {
		//更改来源地址
		psin->sin_addr.S_un.S_addr = *(u_long *)p;

		//更改来源端口
		u_char * pp = (u_char *)(&(psin->sin_port));
		pp[0]= p[8];
		pp[1] =p[9];

		//更改数据长度
		*lpNumberOfBytesRecvd -= 16;

		//更改buf长度的标示值
		lpBuffers[0].len = lpBuffers[0].len - 16;

		//注意指针+16
		memcpy(lpBuffers[0].buf, lpBuffers[0].buf + 16, *lpNumberOfBytesRecvd);
	}

	return iRet;
}

void init_o_procs() {
	char path[260];
	GetSystemDirectory(path, 260);
	strcat(path, "\\ws2_32.dll");
	hdl=LoadLibrary(path);

	p_o_send = (o_send)GetProcAddress(hdl, "send");
	p_o_sendto = (o_sendto)GetProcAddress(hdl, "sendto");
	p_o_connect = (o_connect)GetProcAddress(hdl, "connect");
	p_o_WSARecv = (o_WSARecv)GetProcAddress(hdl, "WSARecv");
	p_o_WSARecvFrom = (o_WSARecvFrom)GetProcAddress(hdl, "WSARecvFrom");
	p_o_recvfrom = (o_recvfrom)GetProcAddress(hdl, "recvfrom");
	p_o_gethostbyname = (o_gethostbyname)GetProcAddress(hdl, "gethostbyname");
	p_o_closesocket = (o_closesocket)GetProcAddress(hdl, "closesocket");
	//p_o_recv = (o_recv)GetProcAddress(hdl, "recv");
	//	p_o_getprotobyname
	//			= (o_getprotobyname)GetProcAddress(hdl, "getprotobyname");
	// p_o_WSAAccept = (o_WSAAccept)GetProcAddress(hdl, "WSAAccept");
	//	p_o_bind = (o_bind)GetProcAddress(hdl, "bind");
	//	p_o_closesocket = (o_closesocket)GetProcAddress(hdl, "closesocket");
	//	p_o_getpeername = (o_getpeername)GetProcAddress(hdl, "getpeername");
	//	p_o_getsockname = (o_getsockname)GetProcAddress(hdl, "getsockname");
	//	p_o_getsockopt = (o_getsockopt)GetProcAddress(hdl, "getsockopt");
	//	p_o_htonl = (o_htonl)GetProcAddress(hdl, "htonl");
	//	p_o_htons = (o_htons)GetProcAddress(hdl, "htons");
	//	p_o_ioctlsocket = (o_ioctlsocket)GetProcAddress(hdl, "ioctlsocket");
	//	p_o_inet_addr = (o_inet_addr)GetProcAddress(hdl, "inet_addr");
	//	p_o_inet_ntoa = (o_inet_ntoa)GetProcAddress(hdl, "inet_ntoa");
	//	p_o_listen = (o_listen)GetProcAddress(hdl, "listen");
	//	p_o_ntohl = (o_ntohl)GetProcAddress(hdl, "ntohl");
	//	p_o_ntohs = (o_ntohs)GetProcAddress(hdl, "ntohs");
	//	p_o_select = (o_select)GetProcAddress(hdl, "select");
	//	p_o_setsockopt = (o_setsockopt)GetProcAddress(hdl, "setsockopt");
	//	p_o_shutdown = (o_shutdown)GetProcAddress(hdl, "shutdown");
	//	p_o_socket = (o_socket)GetProcAddress(hdl, "socket");
	//	p_o_WSApSetPostRoutine = (o_WSApSetPostRoutine)GetProcAddress(hdl,
	//			"WSApSetPostRoutine");
	//	p_o_WPUCompleteOverlappedRequest
	//			= (o_WPUCompleteOverlappedRequest)GetProcAddress(hdl,
	//					"WPUCompleteOverlappedRequest");
	//	p_o_WSAAccept = (o_WSAAccept)GetProcAddress(hdl, "WSAAccept");
	//	p_o_WSAAddressToStringA = (o_WSAAddressToStringA)GetProcAddress(hdl,
	//			"WSAAddressToStringA");
	//	p_o_WSAAddressToStringW = (o_WSAAddressToStringW)GetProcAddress(hdl,
	//			"WSAAddressToStringW");
	//	p_o_WSACloseEvent = (o_WSACloseEvent)GetProcAddress(hdl, "WSACloseEvent");
	//	p_o_WSAConnect = (o_WSAConnect)GetProcAddress(hdl, "WSAConnect");
	//	p_o_WSACreateEvent
	//			= (o_WSACreateEvent)GetProcAddress(hdl, "WSACreateEvent");
	//	p_o_WSADuplicateSocketA = (o_WSADuplicateSocketA)GetProcAddress(hdl,
	//			"WSADuplicateSocketA");
	//	p_o_WSADuplicateSocketW = (o_WSADuplicateSocketW)GetProcAddress(hdl,
	//			"WSADuplicateSocketW");
	//	p_o_WSAEnumNameSpaceProvidersA
	//			= (o_WSAEnumNameSpaceProvidersA)GetProcAddress(hdl,
	//					"WSAEnumNameSpaceProvidersA");
	//	p_o_WSAEnumNameSpaceProvidersW
	//			= (o_WSAEnumNameSpaceProvidersW)GetProcAddress(hdl,
	//					"WSAEnumNameSpaceProvidersW");
	//	p_o_WSAEnumNetworkEvents = (o_WSAEnumNetworkEvents)GetProcAddress(hdl,
	//			"WSAEnumNetworkEvents");
	//	p_o_WSAEnumProtocolsA = (o_WSAEnumProtocolsA)GetProcAddress(hdl,
	//			"WSAEnumProtocolsA");
	//	p_o_WSAEnumProtocolsW = (o_WSAEnumProtocolsW)GetProcAddress(hdl,
	//			"WSAEnumProtocolsW");
	//	p_o_WSAEventSelect
	//			= (o_WSAEventSelect)GetProcAddress(hdl, "WSAEventSelect");
	//	p_o_WSAGetOverlappedResult = (o_WSAGetOverlappedResult)GetProcAddress(hdl,
	//			"WSAGetOverlappedResult");
	//	p_o_WSAGetQOSByName = (o_WSAGetQOSByName)GetProcAddress(hdl,
	//			"WSAGetQOSByName");
	//	p_o_WSAGetServiceClassInfoA = (o_WSAGetServiceClassInfoA)GetProcAddress(
	//			hdl, "WSAGetServiceClassInfoA");
	//	p_o_WSAGetServiceClassInfoW = (o_WSAGetServiceClassInfoW)GetProcAddress(
	//			hdl, "WSAGetServiceClassInfoW");
	//	p_o_WSAGetServiceClassNameByClassIdA
	//			= (o_WSAGetServiceClassNameByClassIdA)GetProcAddress(hdl,
	//					"WSAGetServiceClassNameByClassIdA");
	//	p_o_WSAGetServiceClassNameByClassIdW
	//			= (o_WSAGetServiceClassNameByClassIdW)GetProcAddress(hdl,
	//					"WSAGetServiceClassNameByClassIdW");
	//	p_o_WSAHtonl = (o_WSAHtonl)GetProcAddress(hdl, "WSAHtonl");
	//	p_o_WSAHtons = (o_WSAHtons)GetProcAddress(hdl, "WSAHtons");
	//	p_o_WSAInstallServiceClassA = (o_WSAInstallServiceClassA)GetProcAddress(
	//			hdl, "WSAInstallServiceClassA");
	//	p_o_WSAInstallServiceClassW = (o_WSAInstallServiceClassW)GetProcAddress(
	//			hdl, "WSAInstallServiceClassW");
	//	p_o_WSAIoctl = (o_WSAIoctl)GetProcAddress(hdl, "WSAIoctl");
	//	p_o_gethostbyaddr = (o_gethostbyaddr)GetProcAddress(hdl, "gethostbyaddr");
	//	p_o_getprotobynumber = (o_getprotobynumber)GetProcAddress(hdl,
	//			"getprotobynumber");
	//	p_o_getservbyname = (o_getservbyname)GetProcAddress(hdl, "getservbyname");
	//	p_o_getservbyport = (o_getservbyport)GetProcAddress(hdl, "getservbyport");
	//	p_o_gethostname = (o_gethostname)GetProcAddress(hdl, "gethostname");
	//	p_o_WSAJoinLeaf = (o_WSAJoinLeaf)GetProcAddress(hdl, "WSAJoinLeaf");
	//	p_o_WSANtohl = (o_WSANtohl)GetProcAddress(hdl, "WSANtohl");
	//	p_o_WSANtohs = (o_WSANtohs)GetProcAddress(hdl, "WSANtohs");
	//	p_o_WSAProviderConfigChange = (o_WSAProviderConfigChange)GetProcAddress(
	//			hdl, "WSAProviderConfigChange");
	//	p_o_WSARecvDisconnect = (o_WSARecvDisconnect)GetProcAddress(hdl,
	//			"WSARecvDisconnect");
	//	p_o_WSARemoveServiceClass = (o_WSARemoveServiceClass)GetProcAddress(hdl,
	//			"WSARemoveServiceClass");
	//	p_o_WSAResetEvent = (o_WSAResetEvent)GetProcAddress(hdl, "WSAResetEvent");
	//	p_o_WSASend = (o_WSASend)GetProcAddress(hdl, "WSASend");
	//	p_o_WSASendDisconnect = (o_WSASendDisconnect)GetProcAddress(hdl,
	//			"WSASendDisconnect");
	//	p_o_WSASendTo = (o_WSASendTo)GetProcAddress(hdl, "WSASendTo");
	//	p_o_WSASetEvent = (o_WSASetEvent)GetProcAddress(hdl, "WSASetEvent");
	//	p_o_WSASetServiceA
	//			= (o_WSASetServiceA)GetProcAddress(hdl, "WSASetServiceA");
	//	p_o_WSASetServiceW
	//			= (o_WSASetServiceW)GetProcAddress(hdl, "WSASetServiceW");
	//	p_o_WSASocketA = (o_WSASocketA)GetProcAddress(hdl, "WSASocketA");
	//	p_o_WSASocketW = (o_WSASocketW)GetProcAddress(hdl, "WSASocketW");
	//	p_o_WSAStringToAddressA = (o_WSAStringToAddressA)GetProcAddress(hdl,
	//			"WSAStringToAddressA");
	//	p_o_WSAStringToAddressW = (o_WSAStringToAddressW)GetProcAddress(hdl,
	//			"WSAStringToAddressW");
	//	p_o_WSAWaitForMultipleEvents = (o_WSAWaitForMultipleEvents)GetProcAddress(
	//			hdl, "WSAWaitForMultipleEvents");
	//	p_o_WSCDeinstallProvider = (o_WSCDeinstallProvider)GetProcAddress(hdl,
	//			"WSCDeinstallProvider");
	//	p_o_WSCEnableNSProvider = (o_WSCEnableNSProvider)GetProcAddress(hdl,
	//			"WSCEnableNSProvider");
	//	p_o_WSCEnumProtocols = (o_WSCEnumProtocols)GetProcAddress(hdl,
	//			"WSCEnumProtocols");
	//	p_o_WSCGetProviderPath = (o_WSCGetProviderPath)GetProcAddress(hdl,
	//			"WSCGetProviderPath");
	//	p_o_WSCInstallNameSpace = (o_WSCInstallNameSpace)GetProcAddress(hdl,
	//			"WSCInstallNameSpace");
	//	p_o_WSCInstallProvider = (o_WSCInstallProvider)GetProcAddress(hdl,
	//			"WSCInstallProvider");
	//	p_o_WSCUnInstallNameSpace = (o_WSCUnInstallNameSpace)GetProcAddress(hdl,
	//			"WSCUnInstallNameSpace");
	//	p_o_WSAAsyncSelect
	//			= (o_WSAAsyncSelect)GetProcAddress(hdl, "WSAAsyncSelect");
	//	p_o_WSAAsyncGetProtoByNumber = (o_WSAAsyncGetProtoByNumber)GetProcAddress(
	//			hdl, "WSAAsyncGetProtoByNumber");
	//	p_o_WSAAsyncGetProtoByName = (o_WSAAsyncGetProtoByName)GetProcAddress(hdl,
	//			"WSAAsyncGetProtoByName");
	//	p_o_WSAAsyncGetServByPort = (o_WSAAsyncGetServByPort)GetProcAddress(hdl,
	//			"WSAAsyncGetServByPort");
	//	p_o_WSAAsyncGetServByName = (o_WSAAsyncGetServByName)GetProcAddress(hdl,
	//			"WSAAsyncGetServByName");
	//	p_o_WSACancelAsyncRequest = (o_WSACancelAsyncRequest)GetProcAddress(hdl,
	//			"WSACancelAsyncRequest");
	//	p_o_WSASetBlockingHook = (o_WSASetBlockingHook)GetProcAddress(hdl,
	//			"WSASetBlockingHook");
	//	p_o_WSAUnhookBlockingHook = (o_WSAUnhookBlockingHook)GetProcAddress(hdl,
	//			"WSAUnhookBlockingHook");
	//	p_o_WSAGetLastError = (o_WSAGetLastError)GetProcAddress(hdl,
	//			"WSAGetLastError");
	//	p_o_WSASetLastError = (o_WSASetLastError)GetProcAddress(hdl,
	//			"WSASetLastError");
	//	p_o_WSACancelBlockingCall = (o_WSACancelBlockingCall)GetProcAddress(hdl,
	//			"WSACancelBlockingCall");
	//	p_o_WSAIsBlocking = (o_WSAIsBlocking)GetProcAddress(hdl, "WSAIsBlocking");
	//	p_o_WSAStartup = (o_WSAStartup)GetProcAddress(hdl, "WSAStartup");
	//	p_o_WSACleanup = (o_WSACleanup)GetProcAddress(hdl, "WSACleanup");
	//	p_o___WSAFDIsSet = (o___WSAFDIsSet)GetProcAddress(hdl, "__WSAFDIsSet");
	//	p_o_WEP = (o_WEP)GetProcAddress(hdl, "WEP");
	//	p_o_WSCWriteNameSpaceOrder = (o_WSCWriteNameSpaceOrder)GetProcAddress(hdl,
	//			"WSCWriteNameSpaceOrder");
	//	p_o_WSCWriteProviderOrder = (o_WSCWriteProviderOrder)GetProcAddress(hdl,
	//			"WSCWriteProviderOrder");
	//	p_o_WSANSPIoctl = (o_WSANSPIoctl)GetProcAddress(hdl, "WSANSPIoctl");
	//	p_o_WSCUpdateProvider = (o_WSCUpdateProvider)GetProcAddress(hdl,
	//			"WSCUpdateProvider");
	//	p_o_getaddrinfo = (o_getaddrinfo)GetProcAddress(hdl, "getaddrinfo");
	//	p_o_freeaddrinfo = (o_freeaddrinfo)GetProcAddress(hdl, "freeaddrinfo");
	//	p_o_getnameinfo = (o_getnameinfo)GetProcAddress(hdl, "getnameinfo");
	//	p_o_WSAAsyncGetHostByAddr = (o_WSAAsyncGetHostByAddr)GetProcAddress(hdl,
	//			"WSAAsyncGetHostByAddr");
	//	p_o_WSAAsyncGetHostByName = (o_WSAAsyncGetHostByName)GetProcAddress(hdl,
	//			"WSAAsyncGetHostByName");
	//	p_o_WSALookupServiceBeginA = (o_WSALookupServiceBeginA)GetProcAddress(hdl,
	//			"WSALookupServiceBeginA");
	//	p_o_WSALookupServiceBeginW = (o_WSALookupServiceBeginW)GetProcAddress(hdl,
	//			"WSALookupServiceBeginW");
	//	p_o_WSALookupServiceEnd = (o_WSALookupServiceEnd)GetProcAddress(hdl,
	//			"WSALookupServiceEnd");
	//	p_o_WSALookupServiceNextA = (o_WSALookupServiceNextA)GetProcAddress(hdl,
	//			"WSALookupServiceNextA");
	//	p_o_WSALookupServiceNextW = (o_WSALookupServiceNextW)GetProcAddress(hdl,
	//			"WSALookupServiceNextW");
}

PVOID war3GamerNameAddr = 0;
/*暂时废弃*/
void changeWar3GamerName() {
	//zr_log_debug_ec(0, "doing changeWar3GamerName  handle=%d,  at %8X",	GetCurrentProcess(), war3GamerNameAddr);
	if (war3GamerNameAddr <= 0) {
		zr_log_debug_ec(0, "Can NOT changeWar3GamerName  handle=%d,  at %8X",
				GetCurrentProcess(), war3GamerNameAddr);
		return;
	}
	SIZE_T wlen;
	BOOL res = WriteProcessMemory(GetCurrentProcess(), war3GamerNameAddr,
			newname, strlen(newname) + 1, &wlen);
	if (!res)
		zr_log_error_ec(GetLastError(), "WriteProcessMemory	");
	return;
}

/*暂时废弃*/
PVOID getWar3GamernameAddr() {
	zr_log_debug_ec(0, "getWar3GamernameAddr, handle = %d", GetCurrentProcess());
	//搜索内存检查游戏玩家名标记
	/* ?AUPrefString@@ */
	byte KEY[] = { (byte)0x3F, (byte)0x41, (byte)0x55, (byte)0x50, (byte)0x72,
			(byte)0x65, (byte)0x66, (byte)0x53, (byte)0x74, (byte)0x72,
			(byte)0x69, (byte)0x6E, (byte)0x67, (byte)0x40, (byte)0x40 };
	PVOID addr = zr_memscan_baseaddr(GetCurrentProcess(), (PVOID)0x500000,
			0x60000000, KEY, sizeof KEY);
	if (addr > 0)
		addr = zr_memscan_baseaddr(GetCurrentProcess(), (PVOID)((DWORD)addr
				+ 0x10000), 0x60000000, KEY, sizeof KEY);
	//传说中这个地址加了0x4120就是名字存放内存地址
	if (addr > 0)
		addr = (PVOID)((UINT)addr + 0x4120);
	return addr;
}

/**
 * 第二个固定名字的地址 CEditBox.cpp
 */
PBYTE getWar3GamernameAddr2() {

	/* CEditBox.cpp 传说中的定位 */
	byte
			xEditBox[] = { (byte)0x43, (byte)0x45, (byte)0x64, (byte)0x69,
					(byte)0x74, (byte)0x42, (byte)0x6F, (byte)0x78, (byte)0x2E,
					(byte)0x63, (byte)0x70, (byte)0x70, (byte)0x0, (byte)0x0,
					(byte)0x0 };

	int bufLen = 15;
	byte * buf = new byte[bufLen];

	/* Memory Address 传说中的地址 */
	PBYTE xBaseAddr = (PBYTE)0x15059F88;
	PBYTE xVerifyAddr = 0x0;

	zr_log_debug_ec(0, "w3EditBoxNameScan Begin...");

	DWORD rlen = 0, wlen = 0;
	stopWar3ChangeNameThread = false;
	while (true) {
		Sleep(100);
		if (stopWar3ChangeNameThread)
			continue;
		ReadProcessMemory(GetCurrentProcess(), xBaseAddr, &xVerifyAddr, 4,
				&rlen);
		if (xVerifyAddr != 0x0) {
			ReadProcessMemory(GetCurrentProcess(), xVerifyAddr + 0x9A, buf,
					bufLen, &rlen);
			if (memcmp(buf, xEditBox, sizeof xEditBox) == 0) {
				//zr_log_debug_ec(0, "Find name addr= %8X, name=%s", xVerifyAddr + 0xB8, xVerifyAddr + 0xB8);
				WriteProcessMemory(GetCurrentProcess(), xVerifyAddr + 0xB8,
						newname, strlen(newname) + 1, &wlen);
				//break;
			}
		}
	}

	delete buf;
	zr_log_debug_ec(0, "Scan Over!");
	return 0;
}

void _changeWar3GamerName(LPVOID lpParam) {
	getWar3GamernameAddr2();
	return;
	/*暂时废弃*/

	//还不完善,解决不了,进主的人的修改名称
	stopWar3ChangeNameThread = false;
	Sleep(5000);
	zr_log_debug_ec(0, "Thead Begin: _changeWar3GamerName");
	while (!stopWar3ChangeNameThread) {
		Sleep(1000);
		//当前进程
		if (war3GamerNameAddr <= 0) {
			war3GamerNameAddr = getWar3GamernameAddr();
			continue;
		}
		zr_log_debug_ec(0, "found WAr3GamerName Addr = %8X", war3GamerNameAddr);
		//changeWar3GamerName();
		break;
	}
	while (!stopWar3ChangeNameThread) {
		if (war3GamerNameAddr <= 0)
			break;
		changeWar3GamerName();
		Sleep(500);
	}
	zr_log_debug_ec(0, "Thead End: _changeWar3GamerName");
}

//************
BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		if (hdl==NULL) {
			zr_log_debug("DllMain init");
			//设置logger file path
			zr_log_set_target(ZR_LOG_TG_FILE);
			char logpath[MAX_PATH];
			GetTempPath(sizeof logpath, logpath);
			strcat(logpath, "gf_dll.log");
			zr_log_set_filepath(logpath);
			zr_log_debug_ec(0, "Logger path = %s", logpath);

			//装载ws2_32.dll 原动态库 
			init_o_procs();

			//装载 gf_props
			gf_props_read(&props);

			//取得游戏玩家nick
			memset(newname, 0, 16);
			strncpy(newname, GB2312ToUTF_8(props.nickname,
					strlen(props.nickname)), 15);

			//改war3名字的Thread
			DWORD far threadID;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_changeWar3GamerName, NULL, 0, &threadID);

			//			my_connect
			//			my_recvfrom
			//			my_send
			//			my_sendto
			//			my_gethostbyname
			//			my_WSARecv
			//			my_WSARecvFrom
			//用Detours注入拦截函数
			HMODULE hModule= LoadLibrary("ws2_32.dll");
			p_o_connect = (o_connect)GetProcAddress(hModule, "connect");
			p_o_recvfrom = (o_recvfrom)GetProcAddress(hModule, "recvfrom");
			p_o_send = (o_send)GetProcAddress(hModule, "send");
			p_o_sendto = (o_sendto)GetProcAddress(hModule, "sendto");
			p_o_gethostbyname = (o_gethostbyname)GetProcAddress(hModule,
					"gethostbyname");
			p_o_WSARecv = (o_WSARecv)GetProcAddress(hModule, "WSARecv");
			p_o_WSARecvFrom = (o_WSARecvFrom)GetProcAddress(hModule,
					"WSARecvFrom");
			p_o_closesocket = (o_closesocket)GetProcAddress(hModule,
					"closesocket");
			//p_o_WSAAccept = (o_WSAAccept)GetProcAddress(hModule, "WSAAccept");

			//p_o_recv = (o_recv)GetProcAddress(hModule, "recv");
			//			p_o_setsockopt
			//					= (o_setsockopt)GetProcAddress(hModule, "setsockopt");

			DetourRestoreAfterWith();
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());

			DetourAttach(&(PVOID&)p_o_connect, my_connect);
			DetourAttach(&(PVOID&)p_o_recvfrom, my_recvfrom);
			DetourAttach(&(PVOID&)p_o_send, my_send);
			DetourAttach(&(PVOID&)p_o_sendto, my_sendto);
			DetourAttach(&(PVOID&)p_o_gethostbyname, my_gethostbyname);
			DetourAttach(&(PVOID&)p_o_WSARecv, my_WSARecv);
			DetourAttach(&(PVOID&)p_o_WSARecvFrom, my_WSARecvFrom);
			DetourAttach(&(PVOID&)p_o_closesocket, my_closesocket);
			//DetourAttach(&(PVOID&)p_o_WSAAccept, my_WSAAccept);
			//DetourAttach(&(PVOID&)p_o_recv, my_recv);

			int error = DetourTransactionCommit();
			if (error == NO_ERROR) {
				zr_log_debug("gf.dll: DetourAttach succ.");
			} else {
				zr_log_error_ec(error, "gf.dll: DetourAttach failed.");
			}
		}
		zr_log_debug("DllMain begin");
	} else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)p_o_connect, my_connect);
		DetourDetach(&(PVOID&)p_o_recvfrom, my_recvfrom);
		DetourDetach(&(PVOID&)p_o_send, my_send);
		DetourDetach(&(PVOID&)p_o_sendto, my_sendto);
		DetourDetach(&(PVOID&)p_o_gethostbyname, my_gethostbyname);
		DetourDetach(&(PVOID&)p_o_WSARecv, my_WSARecv);
		DetourDetach(&(PVOID&)p_o_WSARecvFrom, my_WSARecvFrom);
		//		DetourDetach(&(PVOID&)p_o_recv, my_recv);
		//		DetourDetach(&(PVOID&)p_o_setsockopt, my_setsockopt);

		int error = DetourTransactionCommit();
		if (error != NULL)
			zr_log_error_ec(error, "gf.dll: DetourDetach failed.");
		fflush(stdout);
	}
	return 1;
}