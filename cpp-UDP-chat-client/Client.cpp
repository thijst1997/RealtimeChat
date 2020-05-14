#include "Client.h"
#include "CircularLineBuffer.h"

std::string ipAddress = "127.0.0.1";
std::string port = "4444";

struct addrinfo hints, *results;
bool logedIn = false;

addrinfo host_info = {};
addrinfo *host_info_list = NULL;
sockaddr_in addr_recv = {};
sockaddr_in addr_send = {};

std::string client = "CLIENT> ";
std::string server = "SERVER> ";

const int BUF_SIZE = 1024;
char buf[BUF_SIZE];
std::string userInput;


void Client::tick() {

	if (logedIn == true)
	{

		if (stdinBuffer.hasLine())
		{
			std::string test = stdinBuffer.readLine();
			int sendMes = send(sock, test.c_str(), strlen(test.c_str()), 0);
		}

		if (socketBuffer.hasLine())
		{

			std::cout << socketBuffer.readLine() << std::endl;

		}

	}

}

int Client::readFromStdin() {

	std::cout << client;
	getline(std::cin, userInput);
	std::string cmd = userInput + "\n";
	stdinBuffer.writeChars(cmd.c_str(), strlen(cmd.c_str()));

	if (userInput.substr(0, 1)._Equal("!"))
	{

		clientCommands(userInput);
		
	}

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

	sock = createSocket(ipAddress, port);

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

		std::cout << "enter a message: @username message" << std::endl;
		std::cout << client;
		getline(std::cin, userInput);

		if (userInput.substr(0, 1)._Equal("!"))
		{

			clientCommands(userInput);
			continue;

		}

		std::string message = sendChat(sock, userInput);
		if (!message.empty())
		{
			std::cout << message << std::endl;
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

	std::cout << "Enter a username..." << std::endl;
	getline(std::cin, userInput);

	clientCommands(userInput);

	std::string cmd = "HELLO-FROM " + userInput;

	sendMessage(s, cmd);
	std::string message = recvMessage(s, buf, BUF_SIZE);

	if (message._Equal("IN-USE\n") || message._Equal("BUSY\n"))
	{

		if (message._Equal("IN-USE\n")) {
			std::cout << server << "this nickname is already in use, try a different nickname..." << std::endl;
		}
		else if (message._Equal("BUSY\n")) {
			std::cout << server << "Server is busy, try again in a while..." << std::endl;
		}


		sock = createSocket(ipAddress, port);
		return logedIn = false;
	}
	else {

		std::cout << server << "You're logged in as " << userInput << std::endl;
		return logedIn = true;
	}



}


/**
 * @brief
 * Creates a socket and connects it to the TCP/IP server
 * @param std::string ipAdress;
 * @param std::string port;
 * @return SOCKET; initialized socket
 */
SOCKET Client::createSocket(std::string ipAddress, std::string port) {

	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;  
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	getaddrinfo(ipAddress.c_str(), port.c_str(), &hints, &results);
	memcpy(&addr_send, results->ai_addr, results->ai_addrlen); 
	sock = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

	addr_recv.sin_family = AF_INET;
	
	int bindInt = bind(sock, (sockaddr*) &addr_recv, sizeof(addr_recv));


	//int info = 0)
	//{= getaddrinfo(ipAddress.c_str(), port.c_str(), &hints, &results);
	//if (info !
	//	std::cerr << "Can't get addr info, ERR #" << info << std::endl;
	//}

	//SOCKET sock = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
	//if (sock == INVALID_SOCKET)
	//{
	//	std::cerr << "Can't create socked, ERR #" << WSAGetLastError() << std::endl;
	//}

	//int bindInt = bind(sock, results->ai_addr, results->ai_addrlen);
	//if (bindInt == 0)
	//{
	//	std::cout << "ayy oke" << std::endl;
	//}


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
	std::cout << client << "Closed socket" << std::endl;
	exit(0);

}

/**
 * @brief
 * Recieves message from the TCP/IP server
 * @param SOCKET s; initilized socket
 * @param char* buf; empty char array
 * @param int BUF_SIZE; length of the char array
 * @return std::string; a respons message from the TCP/IP server
 */
std::string Client::recvMessage(SOCKET s, char* buf, int BUF_SIZE) {

	std::string message;

	socklen_t fromlen = sizeof results->ai_addr;
	int byteRecv = recvfrom(s, buf, BUF_SIZE, 0, results->ai_addr, &fromlen);
	if (byteRecv > 0)
	{
		message = std::string(buf, 0, byteRecv);
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
 * @param std::string userInput; protocol message
 */
void Client::sendMessage(SOCKET s, std::string userInput) {

	std::string cmd = userInput + "\n";
	int sendResult = sendto(s, cmd.c_str(), strlen(cmd.c_str()), 0, results->ai_addr, sizeof(results->ai_addrlen));
	if (sendResult >= 0)
	{
		std::cerr << client << "Message send" << std::endl;
	}
	else
	{
		std::cerr << server << "nothing send or error occured, ERR #" << WSAGetLastError() << std::endl;
	}

}

/**
 * @brief
 * cmd !who returns al current online users
 * cmd !quit closes socket, connection and the application
 * @param std::string userInput; if userInput starts with '!' execute this method
 */
void Client::clientCommands(std::string userInput) {
	if (userInput._Equal("!quit"))
	{
		closeApplication(sock);
	}

	if (userInput._Equal("!who"))
	{

		std::string cmd = "WHO";
		sendMessage(sock, cmd);
		std::string message = recvMessage(sock, buf, BUF_SIZE);
		std::cout << message << std::endl;

	}
}

/**
 * @brief
 * Sends a message to a specific user and gets one back
 * @param SOCKET s; initialized socket
 * @return std::string; a message of the recieved response from the server
 */
std::string Client::sendChat(SOCKET s, std::string userInput) {

	std::string message;

	if (userInput.substr(0, 1)._Equal("@"))
	{
		userInput.erase(0, 1);
	}

	std::string cmd = "SEND " + userInput;

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