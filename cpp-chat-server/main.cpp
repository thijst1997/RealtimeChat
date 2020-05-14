#include <iostream>
#include "Server.h"
using namespace std;

#define REQ_WINSOCK_VER 2

int main()
{
	try
	{
		WSockServer server(REQ_WINSOCK_VER);
		if(server.RunServer(4444))
		{
			cout << "Running server. " << endl;
			server.StartChat();
		}
	}
	catch(char *ErrMsg)
	{
		cout << "\nError: " << ErrMsg;
	}
	return 0;
}
