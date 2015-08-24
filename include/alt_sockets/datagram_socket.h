#ifndef ALT_SOCKETS_DATAGRAM_SOCKET_H_
#define ALT_SOCKETS_DATAGRAM_SOCKET_H_

#include "socket.h"

class Datagram_Socket : public Socket
{
public:
	Datagram_Socket();
	virtual ~Datagram_Socket();

    virtual bool isConnected() = 0;
    virtual bool listenOn(uint16_t port,const char * listenOnAddr) = 0;
    virtual bool connectTo(const char * hostname, uint16_t port, uint32_t timeout) = 0;
    virtual bool writeBlock(const void * data, uint32_t datalen) = 0;
    virtual bool readBlock(void * data, uint32_t datalen) = 0;
};

#endif /* ALT_SOCKETS_DATAGRAM_SOCKET_H_ */
