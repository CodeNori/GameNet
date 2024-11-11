#pragma once
#include <winsock2.h>
#include <vector>

class TcpServer
{
public:
	struct Session {
		SOCKET  sock;
		SOCKADDR_IN clientAddr;
	};

public:
	TcpServer(unsigned short port);
	~TcpServer();

	bool AcceptSession();
	bool RecvSession(Session* ss);
	void SendToAll(char* buf, int len);
	void Update();
	void CloseSession(Session* ss);

private:
	bool CreateListenSocket();
	int addSession(Session* client);

	unsigned short mPort;
	SOCKET listen_socket;
	FD_SET mReadSet;
	std::vector<Session*> mSessions;

	int mBufLen;
	char mBuf[4096];


};

