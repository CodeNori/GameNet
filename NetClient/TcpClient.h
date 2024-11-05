#pragma once
#include <winsock2.h>

class TcpClient
{
public:
	TcpClient(const char* ip, unsigned short port);
	~TcpClient();

	bool SandSample();
	bool RecvData();
	bool Select(struct timeval timeout);

private:
	bool CreateClientSocket(const char* ip, unsigned short port);

	SOCKET mSocket;
	unsigned short mPort;
};

