#ifndef ALT_SOCKETS_STREAM_SOCKET_H_
#define ALT_SOCKETS_STREAM_SOCKET_H_

#include "socket.h"
#include <utility>

class Stream_Socket : public Socket
{
public:
	Stream_Socket();
	virtual ~Stream_Socket();

    /**
     * @brief GetSocketPair Create a Pair of interconnected sockets
     * @return pair of interconnected Stream_Sockets.
    */
    std::pair<Stream_Socket,Stream_Socket> GetSocketPair();

	// non implemented methods here:
    virtual bool isConnected();
    virtual bool listenOn(uint16_t port,const char * listenOnAddr, bool useIPv4 , int recvbuffer,unsigned int backlog);
    virtual bool connectTo(const char * hostname, uint16_t port, uint32_t timeout);
    virtual Stream_Socket * acceptConnection();

    /**
     * Virtual function for protocol initialization after the connection starts...
     * useful for TLS server, it runs in blocking mode and should be called apart to avoid tcp accept while block
     * @return returns true if was properly initialized.
     */
    virtual bool PostAcceptSubInitialization();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Basic R/W options:

    /**
     * Write Null Terminated String on the socket
     * note: it writes the '\0' on the socket.
     * @param data null terminated string
     * @return true if the string was successfully sent
     */
    virtual bool writeBlock(const char * data);
    /**
     * Write a data block on the socket
     * Send the data block in chunks until it ends or fail.
     * You can specify sizes bigger than 4k/8k, or megabytes (be careful with memory), and it will be fully sent in chunks.
     * @param data data block.
     * @param datalen data length in bytes
     * @return true if the data block was sucessfully sent.
     */
    virtual bool writeBlock(const void * data, uint32_t datalen);
    /**
     * Read a data block from the socket
     * Receive the data block in 4k chunks (or less) until it ends or fail.
     * You can specify sizes bigger than 4k/8k, or megabytes (be careful with memory), and it will be fully received in chunks.
     * You may want to specify read a timeout for your protocol.
     * @param data data block.
     * @param datalen data length in bytes
     * @return true if the data block was sucessfully received.
     */
    virtual bool readBlock(void * data, uint32_t datalen, uint32_t * bytesReceived = NULL);


};

#endif /* ALT_SOCKETS_STREAM_SOCKET_H_ */
