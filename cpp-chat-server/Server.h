#ifndef __SERVER_H__
#define __SERVER_H__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <iostream>
#include <map>
using namespace std;

class WSockServer
{
public:
	WSockServer(int RequestVersion);
	~WSockServer();

	bool RunServer(int PortNumber);			// Run te server
	void StartChat();						// Start communication
	void loginProtocol(int sock, string msg);
	void whoProtocol(int sock, map<int, string> socketNames);
	void sendProtocol(int sock, map<int, string> socketNames, string msg);
	void badRequestProtocol(int sock, string msg);
private:
	WSADATA wsaData;
	SOCKET ServerSocket;
	SOCKET ClientSocket;
	sockaddr_in sockAddr;
	sockaddr_in ClientAddr;
	map<int, string>  socketNames;
	
	char Buffer[128];
	void SetServerSockAddr(sockaddr_in *sockAddr, int PortNumber);
};


#endif