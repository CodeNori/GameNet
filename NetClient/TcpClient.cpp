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

bool TcpClient::SandSample()
{
	char buf[1024];
	sprintf_s(buf, "%d %d ABCD", mPort, mSocket );
	int len = strlen(buf)+1;
	int r = send(mSocket, buf, len, 0);
	if (r == SOCKET_ERROR) return false;

	printf("Send... \n");
	return true;
}

bool TcpClient::RecvData()
{
	char buf[1024];
	int r = recv(mSocket, buf, 1024, 0);
	if(r>0) {
		buf[r] = 0;
		printf("    recv: %s\n", buf);
		return true;
	}

	return false;
}

bool TcpClient::Select(timeval timeout)
{
    FD_SET rset;

    FD_ZERO(&rset);
    FD_SET(mSocket, &rset);

    int fd_num = select(0, &rset, nullptr, nullptr, &timeout);

	return fd_num > 0;
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

    //u_long on = 1;
    //int r = ioctlsocket(mSocket, FIONBIO, &on);

	int r = connect(mSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (r == SOCKET_ERROR) { return false; }

	SOCKADDR_IN my_addr;
	int addr_len   = sizeof(my_addr);
	getsockname(mSocket, (sockaddr*)&my_addr, &addr_len );
	mPort = ntohs(my_addr.sin_port);

	printf("Connect 성공\n");
	return true;
}
