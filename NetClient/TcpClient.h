#pragma once
#include <winsock2.h>

class TcpClient
{
public:
	TcpClient(const char* ip, unsigned short port);
	~TcpClient();

	void SandSample();

private:
	bool CreateClientSocket(const char* ip, unsigned short port);

	SOCKET mSocket;

};

