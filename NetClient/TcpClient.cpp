#include "TcpClient.h"
#include <stdio.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32")

bool g_TcpNetworkInited = false;

bool Init_TCP() {
	if (g_TcpNetworkInited) return true;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return true;

	return false;
}

void Close_TCP() {
	if (g_TcpNetworkInited) WSACleanup();

	g_TcpNetworkInited = false;
}

TcpClient::TcpClient(const char* ip, unsigned short port)
{
	printf("클라이언트 생성자\n");
	Init_TCP();
	CreateClientSocket(ip, port);
}

TcpClient::~TcpClient()
{
	printf("클라이언트 소멸\n");
	closesocket(mSocket);
	Close_TCP();
}

void TcpClient::SandSample()
{
	char buf[1024];
	strcpy_s(buf, "1234567890");
	int len = strlen(buf);
	int r = send(mSocket, buf, len, 0);
}

bool TcpClient::CreateClientSocket(const char* ip, unsigned short port)
{
	mSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mSocket == INVALID_SOCKET) { return false; }

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(port);

	//AF_INET, &clientaddr.sin_addr, ipstr, sizeof(ipstr)

	int r = connect(mSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (r == SOCKET_ERROR) { return false; }

	printf("Connect 성공\n");
	return true;
}
