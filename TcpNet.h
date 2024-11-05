#pragma once
#include "preDefines.h"
#include <winsock2.h>

class TcpClient
{
public:
    static TcpClient* create(unsigned short Port);

    TcpClient();
    ~TcpClient();

};


class TcpServer
{
public:
    struct Session {
        SOCKET  sock;
        SOCKADDR_IN clientAddr;
    };

private:
    bool _creatListenSocket();
    int _acceptSession();
    int _recvSession(Session* ss);
    void _sendAll(char* buf, int len);

public:

    static TcpServer* create(unsigned short Port);

    TcpServer();
    ~TcpServer();
    void update();

public:
    unsigned short mPort;
    SOCKET mListenSock;
    FD_SET mRead_fds;

    void addSession(Session* client);
    void deleteSession(Session* client);
    std::vector<Session*> mSessions;
};








