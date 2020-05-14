#ifndef CPP_CHAT_CLIENT_CLIENT_H
#define CPP_CHAT_CLIENT_CLIENT_H

#ifdef _WIN32
#else
#include <pthread.h>
#endif

#include <thread>
#include <string.h>
#include "Application.h"
#include "socket.h"
#include "CircularLineBuffer.h"

class Client : public Application {
private:

    int sock;
    std::thread socketThread, stdinThread;
    CircularLineBuffer socketBuffer, stdinBuffer;
	

    /**
     * @return -1 if anything went wrong, or when the user types '!exit'. 0 otherwise.
     */
    void tick() override;


	/**
	 * @return Returns -1 if the user writes '!exit', returns the number of characters read otherwise.
	 */
    int readFromStdin();

    /**
     * @return the return value of 'recv'.
     */
    int readFromSocket();

    inline void threadReadFromStdin() {
        while (!isStopped()) {
            auto res = readFromStdin();
            if (res < 0) {
                stopApplication();
            }
        }
    }

    inline void threadReadFromSocket() {
        while (!isStopped()) {
            auto res = readFromSocket();
            if (res < 0) {
                stopApplication();
            }
        }
    }

   /**
     * Creates a socket connection to a listening server and login with a username
     *
     */
    void createSocketAndLogIn();

    /**
     * Cleans and closes the socket connection
     */
    void closeSocket();

	 void checkInput();

	bool promtLogin(SOCKET s);

	SOCKET createSocket(std::string ipAddress, std::string port);

	void closeApplication(SOCKET s);

	std::string recvMessage(SOCKET s, char * buf, int BUF_SIZE);

	void sendMessage(SOCKET s, std::string userInput);

	void clientCommands(std::string userInput);

	std::string sendChat(SOCKET s, std::string userInput);

    inline void startThreads() {
        socketThread = std::thread(&Client::threadReadFromSocket, this);
        stdinThread = std::thread(&Client::threadReadFromStdin, this);
    }

    inline void stopThreads() {
        this->stopApplication();
        socketThread.join();
        stdinThread.join();
    }

public:
    inline ~Client() override {
        closeSocket();
        stopThreads();
    }

    inline void setup() override {
        createSocketAndLogIn();
        startThreads();
    }
};


#endif //CPP_CHAT_CLIENT_CLIENT_H
