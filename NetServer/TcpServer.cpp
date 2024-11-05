#include "TcpServer.h"
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

TcpServer::TcpServer(unsigned short port)
{
	printf("서버 생성자\n");
    Init_TCP();
    mPort = port;
    CreateListenSocket();
    
}

TcpServer::~TcpServer()
{
	printf("서버 소멸\n");
    closesocket(listen_socket);
    Close_TCP();
}

bool TcpServer::CreateListenSocket()
{
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == INVALID_SOCKET) { return false; }

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(mPort);

    int r = bind(listen_socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (r == SOCKET_ERROR) { return false; }

    r = listen(listen_socket, 10);
    if (r == SOCKET_ERROR) return false;

    u_long on = 1;
    r = ioctlsocket(listen_socket, FIONBIO, &on);

    FD_ZERO(&mReadSet);
    FD_SET(listen_socket, &mReadSet);

    printf("Listen 성공 (%d)\n", listen_socket );
    return true;
}

void TcpServer::addSession(Session* client)
{
    for (auto& ss : mSessions) {
        if (nullptr == ss) {
            ss = client;
            return;
        }
    }

    mSessions.push_back(client);
}

bool TcpServer::AcceptSession()
{
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen = sizeof(clientaddr);

    client_sock = accept(listen_socket, (SOCKADDR*)&clientaddr, &addrlen);
    if (client_sock == INVALID_SOCKET) {
        return false;
    }

    char ipstr[32] = { 0, };
    inet_ntop(AF_INET, &clientaddr.sin_addr, ipstr, sizeof(ipstr));

    printf("\nSession 접속(%d): IP=%s, Port=%d\n",
        client_sock, ipstr, ntohs(clientaddr.sin_port));


    Session* ss = new Session;
    ss->sock = client_sock;
    ss->clientAddr = clientaddr;

    addSession(ss);

    FD_SET(client_sock, &mReadSet);

    return true;
}

bool TcpServer::RecvSession(Session* ss)
{
    int r = recv(ss->sock, mBuf, 1024, 0);

    if (r<=0) return false;

    mBuf[r] = 0;
    mBufLen = r;
    printf("RECV(%d) : %s\n",ss->sock, mBuf);
        
    return true;
}

void TcpServer::SendToAll(char* buf, int len)
{
    for (auto& ss : mSessions)
    {
        if (ss && ss->sock != INVALID_SOCKET)
        {
            int r = send(ss->sock, buf, len, 0);
            printf("    send to (%d)..\n", ss->sock);
        }
    }
}

void TcpServer::Update()
{
    FD_SET rset= mReadSet;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    /*
    FD_ZERO(&rset);
    FD_SET(listen_socket, &rset);
    for (auto& ss : mSessions)
    {
        if (ss && ss->sock != INVALID_SOCKET)
        {
            FD_SET(ss->sock, &rset);
        }
    }
    */

    int fd_num = select(0, &rset, nullptr, nullptr, &timeout);

    if (fd_num > 0) {

        // 데이터 온놈 recv하기
        for (auto& ss : mSessions)
        {
            if (ss && FD_ISSET(ss->sock, &rset))
            {
                bool r = RecvSession(ss);
                if (r) {
                    SendToAll(mBuf, mBufLen);
                } else { 
                    CloseSession(ss); 
                }
            }
        }

        // 네트워크 죽은놈 제거
        for (auto& ss : mSessions)
        {
            if (ss && ss->sock == INVALID_SOCKET) {
                delete ss;
                ss = nullptr;
            }
        }

        // connet 처리해주기
        if (FD_ISSET(listen_socket, &rset))
        {
            AcceptSession();
        }
    }
}

void TcpServer::CloseSession(Session* ss)
{
    if (ss->sock != INVALID_SOCKET)
    {
        printf("Session 종료 : %d\n", ss->sock);
        FD_CLR(ss->sock, &mReadSet);

        closesocket(ss->sock);
        ss->sock = INVALID_SOCKET;
    }
}
