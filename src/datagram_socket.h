#ifndef ALT_SOCKETS_DATAGRAM_SOCKET_H_
#define ALT_SOCKETS_DATAGRAM_SOCKET_H_

#include "socket.h"
#include <memory>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

struct DatagramBlock
{
    DatagramBlock()
    {
        data = nullptr;
        datalen = -1;
    }
    ~DatagramBlock()
    {
        Free();
    }
    void Free()
    {
        if (data) delete [] data;
    }
    void Copy(void * _data, int dlen)
    {
        if (dlen>0 && dlen<1024*1024) // MAX: 1Mb.
        {
            Free();
            data = new unsigned char[dlen];
            memcpy(data,_data,dlen);
        }
    }
    struct sockaddr addr;
    unsigned char * data;
    int datalen;
};


class Datagram_Socket : public Socket
{
public:
	Datagram_Socket();
	virtual ~Datagram_Socket();

    // Datagram Specific Functions.
    virtual std::shared_ptr<DatagramBlock> readDatagramBlock() = 0;

    // Socket specific functions:
    virtual bool isConnected() = 0;
    virtual bool listenOn(uint16_t port,const char * listenOnAddr) = 0;
    virtual bool connectTo(const char * hostname, uint16_t port, uint32_t timeout) = 0;
    virtual bool writeBlock(const void * data, uint32_t datalen) = 0;
    virtual bool readBlock(void * data, uint32_t datalen) = 0;
};

#endif /* ALT_SOCKETS_DATAGRAM_SOCKET_H_ */
