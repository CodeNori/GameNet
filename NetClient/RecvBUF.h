#pragma once
#include <winsock2.h>

class RecvBUF
{
public:
	int Recv(SOCKET s);
	char* GetPacket();
	void End();

	char mBuf[4096];
	int mFirst;
	int mLast;
};


void aaaa()
{
	RecvBUF rv;
	if (rv.Recv(s)) {
		char* data = nullptr;
		while(data = rv.GetPacket())
		{

		} ;
		rv.End();
	}

}
