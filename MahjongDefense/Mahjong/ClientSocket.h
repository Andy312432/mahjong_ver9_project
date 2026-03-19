#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#define MAXRECV 256

#include <winsock2.h>
#ifdef _MSC_VER
#pragma comment(lib,"ws2_32.lib")
#endif
//#pragma comment(lib,"libws2_32.a")

using namespace std;

class ClientSocket {
public:
	ClientSocket() {};
	ClientSocket(const string& sIp, const int& iPort) { initSocket(sIp, iPort); };

public:
	inline void closeSocket() const { closesocket(m_sock); };

	bool initSocket(const string& sIp, const int& iPort);
	void sendData(const string& sData) const;
	void recvData(string& sData) const;
	void sendData(const vector<float>& vData);
	void recvData(vector<float>& vData);
	void sendData(const int& iData);
	void recvData(int& iData);
	const ClientSocket& operator<<(const string&) const;
	const ClientSocket& operator>>(string&) const;

private:
	SOCKET m_sock = INVALID_SOCKET;;
};

#endif
