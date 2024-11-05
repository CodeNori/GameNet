#include "TcpClient.h"
#include <stdio.h>
#pragma comment(lib, "ws2_32")

TcpClient::TcpClient()
{
	printf("클라이언트 생성자\n");
}

TcpClient::~TcpClient()
{
	printf("클라이언트 소멸\n");
}
