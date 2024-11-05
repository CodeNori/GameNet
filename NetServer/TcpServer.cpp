#include "TcpServer.h"
#include <stdio.h>
#pragma comment(lib, "ws2_32")

TcpServer::TcpServer()
{
	printf("서버 생성자\n");
}

TcpServer::~TcpServer()
{
	printf("서버 소멸\n");
}
