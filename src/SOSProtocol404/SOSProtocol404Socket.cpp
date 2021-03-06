/*
 * SOSProtocol404.cpp
 *
 */

#include "SOSProtocol404Socket.h"
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <string.h>
#include <ctgmath>

using namespace std;

SOSProtocol404_Socket::SOSProtocol404_Socket()
{
    this->sock = nullptr;
}

SOSProtocol404_Socket::~SOSProtocol404_Socket()
{
}

void SOSProtocol404_Socket::setSocket(Stream_Socket *sock)
{
    this->sock = sock;
}

unsigned char SOSProtocol404_Socket::readU8(bool* readOK)
{
    if (!this->sock) 
    {
        if (readOK) *readOK=false;
        return 0;
    }
    
	unsigned char rsp[1] =
	{ 0 };
	if (readOK)
		*readOK = true;
	// Receive 1 byte, if fails, readOK is setted as false.
    uint32_t r;
    if ((!sock->readBlock(&rsp, 1, &r) || r!=1) && readOK)
		*readOK = false;
	return rsp[0];
}

bool SOSProtocol404_Socket::writeU8(const unsigned char& c)
{
    if (!this->sock) return false;
	unsigned char snd[1];
	snd[0] = c;
    return sock->writeBlock(&snd, 1); // Send 1-byte
}

uint16_t SOSProtocol404_Socket::readU16(bool* readOK)
{
    if (!this->sock) 
    {
        if (readOK) *readOK=false;
        return 0;
    }
    
	uint16_t ret = 0;
	if (readOK)
		*readOK = true;
	// Receive 2 bytes (unsigned short), if fails, readOK is setted as false.
    uint32_t r;
    if ((!sock->readBlock(&ret, sizeof(uint16_t), &r) || r!=sizeof(uint16_t)) && readOK)
		*readOK = false;
	ret = ntohs(ret); // Reconvert into host based integer.
	return ret;
}

bool SOSProtocol404_Socket::writeU16(const uint16_t& c)
{
    if (!this->sock) return false;
	// Write 16bit unsigned integer as network short.
	uint16_t nbo;
	nbo = htons(c);
    return sock->writeBlock(&nbo, sizeof(uint16_t));
}

uint32_t SOSProtocol404_Socket::readU32(bool* readOK)
{
    if (!this->sock) 
    {
        if (readOK) *readOK=false;
        return 0;
    }
    
    
	uint32_t ret = 0;
	if (readOK)
		*readOK = true;
	// Receive 4 bytes (unsigned int), if fails, readOK is setted as false.

    uint32_t r;
    if ((!sock->readBlock(&ret, sizeof(uint32_t), &r) || r!=sizeof(uint32_t)) && readOK)
		*readOK = false;
	ret = ntohl(ret); // Reconvert into host based integer.
	return ret;
}

bool SOSProtocol404_Socket::writeU32(const uint32_t& c)
{
    if (!this->sock) return false;
	// Write 32bit unsigned integer as network long.
	uint32_t nbo;
	nbo = htonl(c);
    return sock->writeBlock(&nbo, sizeof(unsigned int));
}

bool SOSProtocol404_Socket::writeBlock32(const void* data, uint32_t datalen)
{
    if (!this->sock) return false;
	if (!writeU32(datalen))
		return false;
    return (sock->writeBlock(data, datalen));
}

bool SOSProtocol404_Socket::readBlock32(void* data, uint32_t datalen, bool keepDataLen)
{
    if (!this->sock) 
    {
        return false;
    }
    
	bool readOK;
	uint32_t len;
	if ((len = readU32(&readOK)) != 0 && readOK)
	{
		if (len != datalen && !keepDataLen)
			return false;
        uint32_t r;
        return (sock->readBlock(data, len, &r) && r==len);
	}
	return false;
}

void* SOSProtocol404_Socket::readBlock32WAlloc(unsigned int* datalen)
{
    if (!this->sock) 
    {
        return nullptr;
    }
    
	bool readOK;
	uint32_t len;
	if ((len = readU32(&readOK)) != 0 && readOK)
	{
		if (*datalen < len)  // len received exceeded the max datalen permited.
		{
			*datalen = 0;
			return nullptr;
		}
		// download and resize
		unsigned char * odata = new unsigned char[len];
		if (!odata)
            return nullptr; // not enough memory.
        uint32_t r;
        bool ok = sock->readBlock(odata, len,&r) && r==len;
		if (!ok)
		{
			delete [] odata;
			*datalen = 0;
			return nullptr;
		}
		return odata;
	}
	*datalen = 0;
	return nullptr;
}

/* [0][1][2]
 *  a  b  c char
 *  1  2  3 pos
 *
 *
 */

int SOSProtocol404_Socket::read64KBlockDelim(unsigned char * block, unsigned char* delim, uint16_t delimBytes, unsigned int blockNo)
{
    if (!this->sock) 
    {
        return -1;
    }
    
	bool readOK;

	for (unsigned int pos=1; pos<=65536; pos++)
	{
		block[pos-1]=readU8(&readOK);
		if (!readOK) return -2; // FAILED WITH READ ERROR.

		if ((blockNo==0 && pos>=delimBytes) || blockNo>0)
		{
			unsigned char * comparisson_point = block+(pos-1)-(delimBytes-1);
			if (memcmp(comparisson_point, delim, delimBytes)==0) return (int)pos;
		}
	}
	return -1; // not found.
}

void* SOSProtocol404_Socket::readBlock32WAllocAndDelim(unsigned int* datalen,
        unsigned char* delim, uint16_t delimBytes)
{
    if (!this->sock) 
    {
        return nullptr;
    }
    
	if (*datalen<=65535) return nullptr; // It should at least have 65k of buffer.

	unsigned char * currentBlock = new unsigned char[65536];
	unsigned int currentBlockSize = 65536;
	unsigned int blockNo = 0;

	while (true)
	{
		int bytesRecv = read64KBlockDelim(currentBlock+currentBlockSize-65536, delim, delimBytes, blockNo);
		if (bytesRecv == -2)
		{
			// maybe connection closed... returning nullptr;
			delete [] currentBlock;
			return nullptr;
		}
		else if (bytesRecv == -1)
		{
			if (currentBlockSize+65536>*datalen)
			{
				// Can't request more memory. erase current...
				delete [] currentBlock;
				return nullptr;
			}
			else
			{
				// Requesting more memory... and copying the old block into the new block.
				unsigned char * nextBlock = new unsigned char[currentBlockSize+65536];
				blockNo++;
				memcpy(nextBlock, currentBlock, currentBlockSize);
				delete [] currentBlock;
				currentBlock = nextBlock;
				currentBlockSize+=65536;
			}
		}
		else if (bytesRecv>0)
		{
			*datalen = currentBlockSize-65536+bytesRecv;
			return currentBlock;
		}
	}
}

bool SOSProtocol404_Socket::writeBlock16(const void* data, uint16_t datalen)
{
    if (!this->sock) return false;
	if (!writeU16(datalen))
		return false;
    return (sock->writeBlock(data, datalen));
}

bool SOSProtocol404_Socket::readBlock16(void* data, uint16_t datalen,
        bool keepDataLen)
{
    if (!this->sock) 
    {
        return false;
    }
    
	bool readOK;
	uint16_t len;
	if ((len = readU16(&readOK)) != 0 && readOK)
	{
		if (len != datalen && !keepDataLen)
			return false;
        uint32_t r;
        return sock->readBlock(data, len, &r) && r==len;
	}
	return false;
}

void* SOSProtocol404_Socket::readBlockWAlloc(uint32_t *datalen, unsigned char sizel)
{
    if (!this->sock) 
    {
        return nullptr;
    }
    
    if (!datalen) return nullptr;

    bool readOK = false;
    uint32_t lenReceived = 0;

    if (sizel==8)
        lenReceived = (readU8(&readOK))+1;
    if (sizel==16)
        lenReceived = (readU16(&readOK))+1;
    if (sizel>16)
        lenReceived = (readU32(&readOK))+1;

    if (readOK)
	{
        if (lenReceived > *datalen)  // len received exceeded the max datalen permited.
		{
			*datalen = 0;
			return nullptr;
		}
        *datalen = lenReceived-1;
		// download and resize
        unsigned char * odata = new unsigned char[lenReceived];
        odata[lenReceived-1]=0;
        if (!odata) return nullptr; // not enough memory.
        uint32_t r;
        bool ok = sock->readBlock(odata, lenReceived-1, &r) && r==lenReceived-1;
		if (!ok)
		{
            delete [] odata;
			*datalen = 0;
			return nullptr;
		}
		return odata;
	}
    else
    {
        *datalen = 0;
        return nullptr;
    }
}

bool SOSProtocol404_Socket::writeBlock8(const void* data, uint8_t datalen)
{
    if (!this->sock) return false;
	if (!writeU8(datalen)) 
        return false;
    return (sock->writeBlock(data, datalen));
}

bool SOSProtocol404_Socket::readBlock8(void* data, uint8_t datalen,
        bool keepDataLen)
{
    if (!this->sock) 
    {
        return false;
    }
    
	bool readOK;
	uint8_t len;
	if ((len = readU8(&readOK)) != 0 && readOK)
	{
		if (len != datalen && !keepDataLen)
			return false;
        uint32_t r;
        return sock->readBlock(data, len, &r) && r==len;
	}
    return false;
}

string SOSProtocol404_Socket::readString(bool *readOK, unsigned char sizel)
{
    if (!this->sock) 
    {
        if (readOK) *readOK=false;
        return "";
    }
    
    uint32_t receivedBytes = pow(2,(unsigned int)sizel)-1;

    if (readOK) *readOK = true;

    char * data = (char *)readBlockWAlloc(&receivedBytes,sizel);
    if (!data)
    {
        if (readOK) *readOK = false;
        return "";
    }

    if (!receivedBytes)
    {
        delete [] data;
        return "";
    }
    else
    {
        std::string v(data,receivedBytes);
        delete [] data;
        return v;
    }
}

bool SOSProtocol404_Socket::writeString32(const string &str, uint32_t maxSize)
{
    if (!this->sock) return false;
	if (str.size()>maxSize) return false;
	return writeBlock32(str.c_str(), str.size());
}

bool SOSProtocol404_Socket::writeString16(const std::string& str)
{
    if (!this->sock) return false;
	if (str.size()>65535) return false;
	return writeBlock16(str.c_str(), str.size());
}

bool SOSProtocol404_Socket::writeString8(const std::string& str)
{
    if (!this->sock) return false;
	if (str.size()>255) return false;
	return writeBlock8(str.c_str(), str.size());
}
