#ifndef SOCKET_UNIX_H
#define SOCKET_UNIX_H

#include "stream_socket.h"

/**
 * Unix Socket Class
 */
class  Socket_UNIX : public Stream_Socket {
public:
	/**
	 * Class constructor.
	 */
    Socket_UNIX();
    /**
     * Check if the remote pair is connected or not.
     * @param true if is it connected
     */
    bool isConnected();
    /**
     * Listen on an specific path and address
     * @param listenOnAddress listening path
     * @param port unused parameter.
     * @return true if we can bind to that path.
     */
    bool listenOn(uint16_t port,const char * listenOnAddr);
    /**
     * Connect to remote host using an UNIX socket.
     * @param hostname local path to connect to.
     * @param port 16-bit unsigned integer with the remote port
     * @param timeout timeout in seconds to desist the connection.
     * @return true if successfully connected
     */
    bool connectTo(const char * hostname, uint16_t port, uint32_t timeout = 0);
    /**
     * Accept a new connection on a listening socket.
     * @return returns a socket with the new connection.
     */
    Stream_Socket acceptConnection();
};


#endif // SOCKET_UNIX_H
