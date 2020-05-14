#include "Client.h"
#include "CircularLineBuffer.h"
using namespace std;

string ipAddress = "127.0.0.1";
string port = "4444";

struct addrinfo hints, *results;
bool logedIn = false;

string client = "CLIENT> ";
string server = "SERVER> ";

const int BUF_SIZE = 1024;
char buf[BUF_SIZE];
string userInput;

char* test;

void Client::tick() {

	if (logedIn == true)
	{

		if (stdinBuffer.hasLine())
		{
			string test = stdinBuffer.readLine();
			int sendMes = send(sock, test.c_str(), strlen(test.c_str()), 0);

		}

		if (socketBuffer.hasLine())
		{

			cout << socketBuffer.readLine() << endl;

		}

	}

}

int Client::readFromStdin() {

	cout << client;
	getline(cin, userInput);
	string cmd = userInput + "\n";
	stdinBuffer.writeChars(cmd.c_str(), strlen(cmd.c_str()));

	if (userInput._Equal("!exit"))
	{
		return -1;
	}
	else {
		return sizeof(stdinBuffer);
	}

}

int Client::readFromSocket() {

	int numbytes = recv(sock, buf, BUF_SIZE, 0);
	socketBuffer.writeChars(buf, numbytes);
	return numbytes;

}


void Client::createSocketAndLogIn() {

	sock = createSocket(ipAddress, port, hints, results);

	while (logedIn == false) {

		if (promtLogin(sock) == true)
		{
			logedIn = true;
		}
		else
		{
			logedIn = false;
		}

	}

}

void Client::closeSocket() {

}

void Client::checkInput() {

	do
	{

		cout << "enter a message: @username message" << endl;
		cout << client;
		getline(cin, userInput);

		if (userInput.substr(0, 1)._Equal("!"))
		{

			clientCommands(userInput);
			continue;

		}

		string message = sendChat(sock, userInput);
		if (!message.empty())
		{
			cout << message << endl;
		}

	} while (userInput.size() > 0);

}
/**
 * @brief
 * Logs in into the TCP/IP server
 * param SOCKET s; intialized socket
 */
bool Client::promtLogin(SOCKET s) {
	bool logedIn;

	cout << "Enter a username..." << endl;
	getline(cin, userInput);

	clientCommands(userInput);

	string cmd = "HELLO-FROM " + userInput;

	sendMessage(s, cmd);
	string message = recvMessage(s, buf, BUF_SIZE);

	if (message._Equal("IN-USE\n") || message._Equal("BUSY\n"))
	{

		if (message._Equal("IN-USE\n")) {
			cout << server << "this nickname is already in use, try a different nickname..." << endl;
		}
		else if (message._Equal("BUSY\n")) {
			cout << server << "Server is busy, try again in a while..." << endl;
		}


		sock = createSocket(ipAddress, port, hints, results);
		return logedIn = false;
	}
	else {

		cout << server << "You're logged in as " << userInput << endl;
		return logedIn = true;
	}



}


/**
 * @brief
 * Creates a socket and connects it to the TCP/IP server
 * @param string ipAdress;
 * @param string port;
 * @return SOCKET; initialized socket
 */
SOCKET Client::createSocket(string ipAddress, string port, addrinfo hints, addrinfo *results) {

	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);

	hints.ai_family = AF_INET;
	hints.ai_protocol = 0;
	hints.ai_socktype = SOCK_STREAM;

	int info = getaddrinfo(ipAddress.c_str(), port.c_str(), &hints, &results);
	if (info != 0)
	{
		cerr << "Can't get addr info, ERR #" << info << endl;
	}

	SOCKET sock = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socked, ERR #" << WSAGetLastError() << endl;
	}

	int connection = connect(sock, results->ai_addr, results->ai_addrlen);
	if (connection != 0)
	{
		cerr << "Can't create connection, ERR #" << WSAGetLastError() << endl;
	}
	else if (connection == 0)
	{
		freeaddrinfo(results);
	}

	return sock;
}

/**
 * @brief
 * Closes and quits socket and the application
 * @param SOCKET s; initilaized socket
 */
void Client::closeApplication(SOCKET s) {

	sock_close(s);
	sock_quit();
	cout << client << "Closed socket" << endl;
	exit(0);

}

/**
 * @brief
 * Recieves message from the TCP/IP server
 * @param SOCKET s; initilized socket
 * @param char* buf; empty char array
 * @param int BUF_SIZE; length of the char array
 * @return string; a respons message from the TCP/IP server
 */
string Client::recvMessage(SOCKET s, char* buf, int BUF_SIZE) {

	string message;

	int byteRecv = recv(s, buf, BUF_SIZE, 0);
	if (byteRecv > 0)
	{
		message = string(buf, 0, byteRecv);
	}
	else
	{
		message = "nothing recieved or error occured, ERR #" + WSAGetLastError();
	}

	return message;
}

/**
 * @brief
 * Sends message to the TCP/IP server
 * @param SOCKET s; initialized socket
 * @param string userInput; protocol message
 */
void Client::sendMessage(SOCKET s, string userInput) {

	string cmd = userInput + "\n";
	int sendResult = send(s, cmd.c_str(), strlen(cmd.c_str()), 0);
	if (sendResult >= 0)
	{
		cerr << client << "Message send" << endl;
	}
	else
	{
		cerr << server << "nothing send or error occured, ERR #" << WSAGetLastError() << endl;
	}

}

/**
 * @brief
 * cmd !who returns al current online users
 * cmd !quit closes socket, connection and the application
 * @param string userInput; if userInput starts with '!' execute this method
 */
void Client::clientCommands(string userInput) {
	if (userInput._Equal("!quit"))
	{
		closeApplication(sock);
	}

	if (userInput._Equal("!who"))
	{

		string cmd = "WHO";
		sendMessage(sock, cmd);
		string message = recvMessage(sock, buf, BUF_SIZE);
		cout << message << endl;

	}
}

/**
 * @brief
 * Sends a message to a specific user and gets one back
 * @param SOCKET s; initialized socket
 * @return string; a message of the recieved response from the server
 */
string Client::sendChat(SOCKET s, string userInput) {

	string message;

	if (userInput.substr(0, 1)._Equal("@"))
	{
		userInput.erase(0, 1);
	}

	string cmd = "SEND " + userInput;

	sendMessage(sock, cmd);
	message = recvMessage(sock, buf, BUF_SIZE);

	if (!message.empty()) {

		if (message._Equal("SEND-OK\n"))
		{

			message = recvMessage(sock, buf, BUF_SIZE);

		}

	}
	else
	{
		message = "no message recieved";
	}

	return message;
}