#include "Receive.h"

Receive::Receive()
{
    mFirst = 0;
    mLast  = 0;
    packetLen = 0;
}

Receive::~Receive()
{

}

bool Receive::Recv(SOCKET sock)
{
    int r = recv(sock, recvBuf + mLast, MAX_RECVBUF - mLast - 1, 0);
    if (r < 0) { return false; }

    mLast += r;

    return true;
}

char* Receive::GetPacket()
{
    if (CheckPacketLen())
    {
        if (recvBuf[mFirst] != ID)
        {
            mFirst += packetLen;
            return nullptr;
        }

        char* data = recvBuf + mFirst + 1 + SIZE_PACKETSIZENUM;
        memcpy(packet, data, packetLen - 1 - SIZE_PACKETSIZENUM);
        mFirst += packetLen;

        return packet;
    }
    
    return nullptr;
}

bool Receive::CheckPacketLen()
{
    PK_ID_SIZE idsize;
    memcpy(&idsize, recvBuf + mFirst, sizeof(PK_ID_SIZE));
    packetLen = sizeof(PK_ID_SIZE);

    if (mLast - mFirst < packetLen)
    { return false; }

    return true;
}

void Receive::End()
{
    char b[MAX_RECVBUF] = { 0, };
    memcpy(b, recvBuf, MAX_RECVBUF);

    char n[MAX_RECVBUF] = { 0, };
    memcpy(recvBuf, n, MAX_RECVBUF);

    memcpy(recvBuf, b + mFirst, MAX_RECVBUF - mFirst);
    mLast -= mFirst;
    mFirst = 0;
}
