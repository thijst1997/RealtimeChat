#include "Server.h"
#include <map>
#include <vector>
#include <algorithm> 

WSockServer::WSockServer(int RequestVersion)
{
	ServerSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	if (WSAStartup(MAKEWORD(RequestVersion, 0), &wsaData) == 0)		// Check required version
	{
		if (LOBYTE(wsaData.wVersion < RequestVersion))
		{
			throw "Requested version not available.";
		}
	}
	else
		throw "Startup failed.";
}

WSockServer::~WSockServer()
{
	if (WSACleanup() != 0)
		throw "Cleanup failed.";
	if (ServerSocket != INVALID_SOCKET)
		closesocket(ServerSocket);
	if (ClientSocket != INVALID_SOCKET)
		closesocket(ServerSocket);
}


void WSockServer::SetServerSockAddr(sockaddr_in *sockAddr, int PortNumber)
{
	sockAddr->sin_family = AF_INET;
	sockAddr->sin_port = htons(PortNumber);
	sockAddr->sin_addr.S_un.S_addr = INADDR_ANY;			// Listen on all available ip's
}


bool WSockServer::RunServer(int PortNumber)
{

	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		throw "Could not create socket.";
	}

	SetServerSockAddr(&sockAddr, PortNumber);	// Fill sockAddr

	int bindResult = bind(ServerSocket, (sockaddr*)(&sockAddr), sizeof(sockAddr));
	if (bindResult != 0) {
		throw "Could not bind socket.";
	}

	int listenResult = listen(ServerSocket, SOMAXCONN);
	if (listenResult != 0) {
		throw "Could not put the socket in listening mode.";
	}

	cout << "Server settings: " << endl;
	cout << "IP: " << inet_ntoa(sockAddr.sin_addr) << endl;
	cout << "PORT: " << ntohs(sockAddr.sin_port) << endl << endl;

	return true;

}

void WSockServer::StartChat()
{
	fd_set master;

	FD_ZERO(&master);
	FD_SET(ServerSocket, &master);

	while (true)
	{

		fd_set mCopy = master;

		int socketCount = select(0, &mCopy, nullptr, nullptr, nullptr);

		for (size_t i = 0; i < socketCount; i++)
		{
			SOCKET sock = mCopy.fd_array[i];

			if (sock == ServerSocket)
			{
				// accept new connection
				SOCKET client = accept(ServerSocket, nullptr, nullptr);

				// add the new connection to the list of connected clients
				FD_SET(client, &master);

			}
			else
			{
				if (sock != ServerSocket)
				{
					char buf[4096];
					ZeroMemory(buf, 4096);

					// recieve message
					int byteRecv = recv(sock, buf, 4096, 0);
					// check socket connection
					if (byteRecv == -1)
					{
						closesocket(sock);
						int sockInt = sock;
						socketNames.erase(sockInt);
						FD_CLR(sock, &master);
					}

					if (byteRecv != 0)
					{
						string msg = string(buf, 0, byteRecv);
						printf(msg.c_str());

						// handle bad requests
						badRequestProtocol(sock, msg);

						// handle login
						if (msg.substr(0, 10)._Equal("HELLO-FROM"))
						{
							loginProtocol(sock, msg);
						}

						// handle who's online
						if (msg.substr(0, 5)._Equal("WHO\n"))
						{
							whoProtocol(sock, socketNames);
						}

						// Sends messages to different clients
						if (msg.substr(0, 4)._Equal("SEND"))
						{
							sendProtocol(sock, socketNames, msg);
						}

					}
				}
				
			}
		}
	}

}


void WSockServer::loginProtocol(int sock, string msg) {

	unsigned first = msg.find(" ");
	unsigned last = msg.find("\n");
	string username = msg.substr(first, last - first);
	
	username.erase(remove(username.begin(), username.end(), ' '), username.end());

	bool inUse = false;
	for (auto& x : socketNames)
	{
		if (x.second == username)
		{
			inUse = true;
		}
	}

	if (socketNames.size() != SOMAXCONN)
	{

		if (inUse != true)
		{
			socketNames[sock] = username;
			string loginMsg = "HELLO " + username;
			send(sock, loginMsg.c_str(), strlen(loginMsg.c_str()), 0);
			return;
		}
		else
		{
			string inUseMsg = "IN-USE\n";
			send(sock, inUseMsg.c_str(), strlen(inUseMsg.c_str()), 0);
			return;
		}
	}
	else {
		string busyMsg = "BUSY\n";
		send(sock, busyMsg.c_str(), strlen(busyMsg.c_str()), 0);
		return;
	}



}


void WSockServer::whoProtocol(int sock, map<int, string>  socketNames) {

	string whoMsg = "WHO-OK ";
	for (auto& x : socketNames)
	{
		whoMsg += x.second + ",";
	}
	whoMsg += "\n";

	send(sock, whoMsg.c_str(), strlen(whoMsg.c_str()), 0);
}

void WSockServer::sendProtocol(int sock, map<int, string> socketNames, string msg) {

	
	std::string delimiter = " ";
	size_t pos = 0;
	std::vector<string> tokens;
	while ((pos = msg.find(delimiter)) != std::string::npos) {
		tokens.push_back(msg.substr(0, pos));
		msg.erase(0, pos + delimiter.length());
	}

	string To = tokens.at(1);
	string From;
	int toSock = NULL;
	for (auto& x : socketNames)
	{
		if (x.second == To)
		{
			toSock = x.first;
		}
		if (x.first == sock)
		{
			From = socketNames[sock];
		}
	}

	if (toSock != NULL)
	{
		string sendMsg = "DELIVERY " + From + " " + msg;
		send(toSock, sendMsg.c_str(), strlen(sendMsg.c_str()), 0);
	}
	else {
		string sendMsg = "UKNOWN\n ";
		send(sock, sendMsg.c_str(), strlen(sendMsg.c_str()), 0);
	}

}

void WSockServer::badRequestProtocol(int sock, string msg) {

	char hdrRequests[4][15] = { "HELLO-FROM", "WHO\n", "SEND" };
	for (size_t i = 0; i < sizeof(hdrRequests); i++)
	{
		
		size_t found = msg.find(hdrRequests[i]);
		if (found > 15)
		{
			continue;
		}

		if (found == std::string::npos)
		{
			}
		else {
			break;
		}
	}

	string bdyRequest = "\n";
	if (msg.find(bdyRequest) == string::npos)
	{
		string badRequesBdyMsg = "BAD-RQST-BODY\n";
		send(sock, badRequesBdyMsg.c_str(), strlen(badRequesBdyMsg.c_str()), 0);
		
	}


}