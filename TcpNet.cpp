#include "pch.h"
#include "TcpNet.h"
#pragma comment(lib, "ws2_32")


bool g_TcpNetworkInited = false;

bool Init_TCP() {
    if(g_TcpNetworkInited) return true;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return true;

    return false;
}

void Close_TCP() {
    if(g_TcpNetworkInited) WSACleanup();

    g_TcpNetworkInited = false;
}


bool TcpServer::_creatListenSocket()
{
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) return false;

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(mPort);
    int r = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
    if (r == SOCKET_ERROR) return false;

    r = listen(listen_sock, SOMAXCONN);
    if (r == SOCKET_ERROR) return false;

    u_long on = 1;
	r = ioctlsocket(listen_sock, FIONBIO, &on);

    FD_SET(listen_sock, &mRead_fds);

    mListenSock = listen_sock;
    return true;
}

int TcpServer::_acceptSession()
{
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen = sizeof(clientaddr);


    client_sock = accept(mListenSock, (SOCKADDR *)&clientaddr, &addrlen);
    if (client_sock == INVALID_SOCKET) {
        return -1;
    }

    printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
    inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

    Session* ss = new Session;
    ss->sock = client_sock;
    ss->clientAddr = clientaddr;

    addSession(ss);
    return 0;
}

int TcpServer::_recvSession(Session* ss)
{
    char buf[1024];

    int r = recv(ss->sock, buf, 1024, 0);
	if (r == SOCKET_ERROR)
	{
        closesocket(ss->sock);
        deleteSession(ss);
        return r;
	}
	else if (r == 0)
	{
        closesocket(ss->sock);
        deleteSession(ss);
        return r;
	}

	// 받은 데이터 출력
    SOCKADDR_IN clientaddr;
    int addrlen = sizeof(clientaddr);
	getpeername(ss->sock, (SOCKADDR *) &clientaddr, &addrlen);
	buf[r] = 0;
	printf("[TCP/%s: %d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);

    _sendAll(buf, r);

    return 0;
}

void TcpServer::_sendAll(char* buf, int len)
{
    for( auto ss : mSessions )
    {
        int r = send(ss->sock,buf,len,0);
        if (r == SOCKET_ERROR) {

        }
    }
}

TcpServer* TcpServer::create(unsigned short Port)
{
    return nullptr;
}

TcpServer::TcpServer()
{
    Init_TCP();

    FD_ZERO(&mRead_fds);

    _creatListenSocket();
}

TcpServer::~TcpServer()
{
    closesocket(mListenSock);
    mListenSock = INVALID_SOCKET;
    Close_TCP();
}

void TcpServer::update()
{
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[1024];

    FD_SET rset = mRead_fds;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int fd_num = select(0, &rset, nullptr,nullptr, &timeout);
    if (fd_num == SOCKET_ERROR) return;

    if (FD_ISSET(mListenSock, &rset))
        _acceptSession();

    for( auto ss : mSessions )
    {
        if(FD_ISSET(ss->sock,&rset))
            _recvSession(ss);
    }

}

void TcpServer::addSession(Session* client)
{
    for( auto& ss : mSessions ) {
        if(nullptr == ss) {
            ss = client;
            return;
        }
    }

    mSessions.push_back(client);
}

void TcpServer::deleteSession(Session* client)
{
    for( auto& ss : mSessions ) {
        if(ss == client) {
            ss = nullptr;
            break;
        }
    }

    delete client;
}

TcpClient::TcpClient()
{
    Init_TCP();
}

TcpClient::~TcpClient()
{
    Close_TCP();
}
