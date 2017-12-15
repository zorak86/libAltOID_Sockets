#ifndef SOCKET_TCP_H
#define SOCKET_TCP_H

#include "stream_socket.h"
#include <unistd.h>

#ifdef _WIN32
#include <ws2tcpip.h>
#endif

/**
 * TCP Socket Class
 */
class Socket_TCP : public Stream_Socket {
public:
	/**
	 * Class constructor.
	 */
    Socket_TCP();
    virtual ~Socket_TCP();
    /**
     * Listen on an specific TCP port and address
     * @param listenOnAddress address to listen on. (use :: for ipv6 or 0.0.0.0 if ipv4)
     * @param port 16-bit unsigned integer with the listening TCP port (0-65535)
     * @return true if we can bind the port.
     */
    bool listenOn(uint16_t port,const char * listenOnAddr = "::", bool useIPv4 =false, int recvbuffer = 0,unsigned int backlog = 10);
    /**
     * Connect to remote host using a TCP socket.
     * @param hostname remote hostname to connect to, can be the hostname or the ip address
     * @param port 16-bit unsigned integer with the remote port
     * @param timeout timeout in seconds to desist the connection.
     * @return true if successfully connected
     */
    bool connectTo(const char * hostname, uint16_t port, uint32_t timeout = 0);
    /**
     * Accept a new TCP connection on a listening socket.
     * @return returns a socket with the new established tcp connection.
     */
    virtual Stream_Socket *acceptConnection();

    /**
     * Virtual function for protocol initialization after the connection starts...
     * useful for TLS server, it runs in blocking mode and should be called apart to avoid tcp accept while block
     * @return returns true if was properly initialized.
     */
    virtual bool PostAcceptSubInitialization();

    /**
     * Set TCP_NODELAY Option
     * Avoid nagle algorithm. Send data immediately
     */
    void setFlag_TCP_NODELAY();

    void overrideReadTimeout(int tout = -1);
    void overrideWriteTimeout(int tout = -1);


protected:
    /**
     * Virtual function for protocol initialization after the connection starts (client-mode)...
     * useful for TLS client, it runs in blocking mode.
     * @return returns true if was properly initialized.
     */
    virtual bool PostConnectSubInitialization();



private:
    bool internalConnect(int sockfd, const struct sockaddr *addr, socklen_t addrlen, uint32_t timeout);
    bool internalPassToBlocking(int sockfd);
    bool internalPassToNonBlocking(int sockfd);

    int ovrReadTimeout,ovrWriteTimeout;
};

#endif // SOCKET_TCP_H
