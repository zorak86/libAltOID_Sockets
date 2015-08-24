#ifndef SOCKET_H
#define SOCKET_H

#include <stdint.h>

//TODO: use shared_ptr

enum SocketType
{
	DATAGRAM_SOCKET,
	STREAM_SOCKET,
	UNINITIALIZED_SOCKET
};

/**
 * Socket base class
 * Manipulates all kind of sockets (udp,tcp,unix, etc)
 */
class Socket {
public:
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors/Destructors/Copy:

	/**
	 * Socket Class Constructor
	 * note: does not initialize the socket
	 */
    Socket();
	/**
	 * Socket Class Constructor based on system socket
     * @param _sockfd socket file descriptor
	 */
    Socket(const int & _sockfd);
    /**
     * Copy constructor
     * Construct a socket using the system socket
     * @param _sockfd socket file descriptor
     */
    Socket & operator=(const int & _sockfd);
    /**
     * Socket Class Destructor
     * note: does not destroy/close the socket (should be done by hand: Close())
     */
    virtual ~Socket();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Socket Options:
    /**
     * Set to use write
     * use write instead send.
     */
    void setUseWrite();
    /**
     * Set Read timeout.
     * @param _timeout timeout in seconds
     */
    bool setReadTimeout(unsigned int _timeout);
    /**
     * Set system buffer size.
     * Use to increase the current reception buffer
     * @param buffsize buffer size in bytes.
     */
    void setRecvBuffer(int buffsize);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Socket Raw Manipulation:
    /**
     * Set socket file descriptor value.
     * This is useful to manipulate already created sockets.
     * @param _sockfd socket file descriptor
     */
    void setSocket(int _sockfd);
    /**
     * Get Current Socket file descriptor
     * @return socket file descriptor
     */
    int getSocket() const;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Socket Status:
    /**
     * Check if we have an initialized socket.
     * @todo check if the socket is a really valid file descriptor
     * @return true if the socket is a valid file descriptor
     */
    bool isSocketSettedUp() const;
    /**
     * Check if the remote pair is connected or not.
     * @param true if is it connected
     */
    virtual bool isConnected();
    /**
     * Get current used port.
     * Useful for TCP/UDP connections, especially on received connections.
     * @param 16-bit unsigned integer with the port (0-65535)
     */
    uint16_t getPort();
    /**
     * Get last error message
     * @param last error message pointer. (static mem)
     */
    char * getLastError() const;
    /**
     * Get remote pair address
     * @param address pair address char * (should contain at least 64 bytes)
     */
    void getRemotePair(char * address) const;
    /**
     * Set remote pair address
     * Used by internal functions...
     * @param address pair address char * (should contain at least 64 bytes)
     */
    void setRemotePair(const char * address);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Socket Connection elements...
    /**
     * Connect to remote host
     * This is a virtual function
     * @param hostname remote hostname to connect to
     * @param port 16-bit unsigned integer with the remote port
     * @param timeout timeout in seconds to desist the connection.
     * @return true if successfully connected
     */
    virtual bool connectTo(const char * hostname, const  uint16_t port, uint32_t timeout);
    /**
     * Try connect until it succeeds.
     */
    void tryConnect(const char * hostname, const  uint16_t port, uint32_t timeout);
    /**
     * Listen on an specific port and address
     * @param listenOnAddress address to listen on. (use :: for ipv6 or 0.0.0.0 if ipv4)
     * @param port 16-bit unsigned integer with the listening port (0-65535)
     * @return true if we can bind the port.
     */
    virtual bool listenOn(uint16_t port, const char * listenOnAddr);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Connection Finalization...
    /**
     * Close the socket file descriptor
     * It also shutdown the connection before closing the file descriptor.
     */
    void closeSocket();
    /**
     * Shutdown the connection
     * Use for terminate the connection. both sides will start to fail to receive/send
     * You should also use Close too.
     */
    int shutdownSocket();
    /**
     * Read a data block from the socket
     * Receive the data block in only one command (without chunks).
     * note that this haves some limitations. some systems can only receive 4k at time.
     * You may want to manage the chunks by yourself.
     * @param data data block.
     * @param datalen data length in bytes
     * @return return the number of bytes read by the socket, zero for end of file and -1 for error.
     */
    virtual int partialRead(void * data, uint32_t datalen);

protected:
    /**
     * Connection type.
     */
    SocketType socketType;
    /**
     * if true, Use write instead send and read instead recv.
     */
    bool useWrite;
    /**
     * pointer to the last error message.
     */
    char * lastError;
    /**
     * buffer with the remote pair address.
     */
    char remotePair[64];
    /**
     * socket file descriptor.
     */
    int sockfd;
};



#endif // SOCKET_H
