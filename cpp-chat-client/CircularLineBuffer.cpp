#include "CircularLineBuffer.h"
using namespace std;

int CircularLineBuffer::freeSpace()
{
	
	return bufferSize - count;

}

bool CircularLineBuffer::isFull()
{

	return count == bufferSize;

}

bool CircularLineBuffer::isEmpty()
{

	return count == 0;

}

int CircularLineBuffer::nextFreeIndex()
{

	return (start + count) % bufferSize;

}

int CircularLineBuffer::findNewline()
{

	int newLine = 0;
	for (int i = 0; i < count; i++)
	{

		if (buffer[start] == '\n')
		{
			return newLine += start;
		}


	}

	return newLine;

}

bool CircularLineBuffer::hasLine()
{

	return buffer[(start + count) - 1] == '\n';
	
}

bool CircularLineBuffer::writeChars(const char * chars, size_t nchars)
{

	if (freeSpace() < nchars)
	{
		return false;
	}

	int nfi = nextFreeIndex();
	for (int i = 0; i < nchars; i++)
	{

		buffer[nfi] = chars[i];
		count++;
		nfi++;

	}

	return true;

}

std::string CircularLineBuffer::readLine()
{

	int newLineAt = findNewline();
	if (newLineAt >= 0)
	{
		string line;
		for (int i = 0; i < count; i++)
		{

			line += buffer[start];
			start++;

		}

		count = 0;
		start = start + 1;
		return line;

	}
	else {

		return std::string();

	}

}
