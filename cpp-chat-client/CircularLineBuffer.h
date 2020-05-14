#ifndef CPP_CHAT_CLIENT_CIRCULARBUFFER_H
#define CPP_CHAT_CLIENT_CIRCULARBUFFER_H


#include <cstdlib>
#include <string>
#include <mutex>


class CircularLineBuffer {
private:

	static const int bufferSize = 4096;
    char buffer[bufferSize] = {0};
	 
    std::mutex mtx;

        /**
     * Use 'start' to keep track of the start of the buffer.
     * Use 'count' to keep track of the current number of characters in the buffer.
     */
    int start = 0, count = 0;
public:



/**
     * @return The amount of free space in the buffer in number of characters.
     */
    int freeSpace();

    /**
     *
     * @return true if and only if (iff) the buffer is full.
     */
    bool isFull();

    /**
     *
     * @return true if and only if (iff) the buffer is empty.
     */
    bool isEmpty();

    /**
     * This method should return the next free spot in the buffer as seen from the current value of 'start'.
     *
     * @return The index of the first free position in the buffer.
     */
    int nextFreeIndex();

    /**
     * The position of the next newline character (\n), as seen from the current value of 'start'.
     *
     * @return The position of the next newline character (\n), as seen from the current value of 'start'.
     */
    int findNewline();

    /**
     * Checks if there is a complete line in the buffer.
     * You can make your life easier by implementing this method using the method above.
     *
     * @return true if and only if (iff) there is at least one complete line in the buffer.
     */
    bool hasLine();

    /**
     * This method writes the given number of characters into the buffer,
    *
     * @param chars Pointer to the characters to write into the buffer.
     * @param nchars The number of characters to write.
     * @return False if there was not enough space in the buffer. True otherwise.
     */
    bool writeChars(const char *chars, size_t nchars);

    /**
     * This method reads a line from the buffer,
     * @return The next string from the buffer. Returns an empty string if the buffer is empty.
     */
    std::string readLine();
	int getStart();
};


#endif //CPP_CHAT_CLIENT_CIRCULARBUFFER_H
