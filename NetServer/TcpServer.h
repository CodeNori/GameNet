#pragma once
#include <winsock2.h>
#include <vector>

class TcpServer
{
public:
	struct Session {
		SOCKET  sock;
		SOCKADDR_IN clientAddr;

		bool RecvData();
	};

public:
	TcpServer(unsigned short port);
	~TcpServer();

	bool AcceptSession();
	void Update();
	void CloseSession(Session* ss);

private:
	bool CreateListenSocket();

	unsigned short mPort;
	SOCKET listen_socket;
	FD_SET mReadSet;

	void addSession(Session* client);
	std::vector<Session*> mSessions;

};

