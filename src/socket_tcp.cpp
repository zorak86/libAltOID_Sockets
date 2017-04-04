#include "socket_tcp.h"

#ifdef _WIN32
#include "win32compat/win32netcompat.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#endif

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////                        TCP                                     /////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

Socket_TCP::Socket_TCP()
{
}

Socket_TCP::~Socket_TCP()
{

}

void Socket_TCP::setFlag_TCP_NODELAY()
{
    if (!isValidSocket()) return;

    int flag = 1;
    setsockopt(getSocket(),IPPROTO_TCP, TCP_NODELAY,(char *) &flag, sizeof(int));
}

bool Socket_TCP::connectTo(const char * hostname, uint16_t port, uint32_t timeout)
{
    char servport[32];
    int rc;
    struct in6_addr serveraddr;
    struct addrinfo hints, *res=NULL;

    memset(&hints, 0x00, sizeof(hints));

#ifdef _WIN32
    hints.ai_flags    = 0;
#else
    hints.ai_flags    = AI_NUMERICSERV;
#endif
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    rc = inet_pton(AF_INET, hostname, &serveraddr);
    if (rc == 1)
    {
        hints.ai_family = AF_INET;
        hints.ai_flags |= AI_NUMERICHOST;
    }
    else
    {
        rc = inet_pton(AF_INET6, hostname, &serveraddr);
        if (rc == 1)
        {
            hints.ai_family = AF_INET6;
            hints.ai_flags |= AI_NUMERICHOST;
        }
    }

    snprintf(servport,32,"%u",port);

    rc = getaddrinfo(hostname, servport, &hints, &res);
    if (rc != 0)
    {
        // Host not found.
        lastError = "Error resolving hostname";
        return false;
    }

    bool connected = false;

    for (struct addrinfo *resiter=res; resiter && !connected; resiter = resiter->ai_next)
    {
        if (getSocket() >=0 ) closeSocket();
        setSocket(socket(res->ai_family, res->ai_socktype, res->ai_protocol));
        if (!isValidSocket())
        {
            lastError = "socket() failed";
            break;
        }

        // Set the read timeout here. (to zero)
        setReadTimeout(0);

        if (internalConnect(getSocket(),resiter->ai_addr, resiter->ai_addrlen,timeout))
        {
            // now it's connected...
            if (PostConnectSubInitialization())
            {
                connected = true;
            }
            else
            {
            	// should disconnect here.
            	shutdownSocket();
                // drop the socket descriptor. we don't need it anymore.
                closeSocket();
            }
            break;
        }
        else
        {
            // drop the current socket... (and free the resource :))
            shutdownSocket();
            closeSocket();
        }
    }

    freeaddrinfo(res);

    if (!connected)
    {
        lastError = "connect() failed";
        return false;
    }

    setReadTimeout(60);
    setWriteTimeout(60);

    return true;
}

Stream_Socket * Socket_TCP::acceptConnection()
{
    int sdconn;

    if (!isValidSocket()) return NULL;

    Stream_Socket * cursocket;

    int32_t clilen;
    struct sockaddr_in cli_addr;
    clilen = sizeof(cli_addr);

    if ((sdconn = accept(getSocket(), (struct sockaddr *) &cli_addr, (socklen_t *)&clilen)) >= 0)
    {
        cursocket = new Socket_TCP;
        // Set the proper socket-
        cursocket->setSocket(sdconn);
        char ipAddr[80];
        inet_ntop(AF_INET, &cli_addr.sin_addr, ipAddr, sizeof(ipAddr));
        cursocket->setRemotePort(ntohs(cli_addr.sin_port));
        cursocket->setRemotePair(ipAddr);
    }
    // Establish the error.
    else
    {
        lastError = "accept() failed";
        return NULL;
    }

    // return the socket class.
    return cursocket;
}

bool Socket_TCP::internalConnect(int sockfd, const sockaddr *addr, socklen_t addrlen, uint32_t timeout)
{
    int res2,valopt;

    // Non-blocking connect with timeout...
    if (!internalPassToNonBlocking(sockfd)) return false;

#ifdef _WIN32
    if (timeout == 0)
    {
        // in windows, if the timeval is 0,0, then it will return immediately.
        // however, our lib state that 0 represent that we sleep for ever.
        timeout = 365*24*3600; // how about 1 year.
    }
#endif

    // Trying to connect with timeout.
    res2 = connect(sockfd, addr, addrlen);
    if (res2 < 0)
    {
        if (errno == EINPROGRESS || !errno)
        {
            fd_set myset;

            struct timeval tv;
            tv.tv_sec = timeout;
            tv.tv_usec = 0;
            FD_ZERO(&myset);
            FD_SET(sockfd, &myset);
            res2 = select(sockfd+1, NULL, &myset, NULL, &tv);
            if (res2 < 0 && errno != EINTR)
            {
                lastError = "Error selecting...";
                return false;
            }
            else if (res2 > 0)
            {
                // Socket selected for write
                socklen_t lon;
                lon = sizeof(int);
#ifdef _WIN32
                if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)(&valopt), &lon) < 0)
#else
                if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0)
#endif
                {
                    lastError = "Error in getsockopt() (??)";
                    return false;
                }
                // Check the value returned...
                if (valopt)
                {
                    lastError = "Error in delayed connection()";
                    return false;
                }

                // Even if we are connected, if we can't go back to blocking, disconnect.
                if (!internalPassToBlocking(sockfd)) return false;

                // Connected!!!
                // Pass to blocking mode socket instead select it.
                return true;
            }
            else
            {
                lastError = "Timeout in select() - Cancelling!";
                return false;
            }
        }
        else
        {
            lastError = "Error connecting - (2)";
            return false;
        }
    }
    // What we are doing here?
    internalPassToBlocking(sockfd);
    return false;
}

bool Socket_TCP::internalPassToBlocking(int sockfd)
{
#ifdef _WIN32
    int iResult;
    u_long iMode = 0;
    iResult = ioctlsocket(sockfd, FIONBIO, &iMode);
    return (iResult == NO_ERROR);
#else
    long arg;
    // Set to blocking mode again...
    if( (arg = fcntl(sockfd, F_GETFL, NULL)) < 0)
    {
        lastError = "Error setting blocking... (1)";
        return false;
    }
    arg &= (~O_NONBLOCK);
    if( fcntl(sockfd, F_SETFL, arg) < 0)
    {
        lastError = "Error setting blocking... (2)";
        return false;
    }
    return true;
#endif
}

bool Socket_TCP::internalPassToNonBlocking(int sockfd)
{
#ifdef _WIN32
    int iResult;
    u_long iMode = 1;
    iResult = ioctlsocket(sockfd, FIONBIO, &iMode);
    return (iResult == NO_ERROR);
#else
    long arg;

    // Set non-blocking
    if( (arg = fcntl(sockfd, F_GETFL, NULL)) < 0)
    {
        lastError = "Error setting non-blocking... (1)";
        return false;
    }
    arg |= O_NONBLOCK;
    if( fcntl(sockfd, F_SETFL, arg) < 0)
    {
        lastError = "Error setting non-blocking... (2)";
        return false;
    }
    return true;
#endif
}

bool Socket_TCP::listenOn(uint16_t port, const char * listenOnAddr, bool useIPv4, int recvbuffer,unsigned int backlog)
{
    int on=1;

    if (useIPv4)
    {
        setSocket(socket(AF_INET, SOCK_STREAM, 0));
        if (!isValidSocket())
        {
            lastError = "socket() failed";
            return false;
        }
    }
    else
    {
        setSocket(socket(AF_INET6, SOCK_STREAM, 0));
        if (!isValidSocket())
        {
            lastError = "socket() failed";
            return false;
        }
    }

    if (recvbuffer) setRecvBuffer(recvbuffer);

    if (setsockopt(getSocket(), SOL_SOCKET, SO_REUSEADDR, (char *)&on,sizeof(on)) < 0)
    {
        lastError = "setsockopt(SO_REUSEADDR) failed";
        closeSocket();
        return false;
    }

    if (useIPv4)
    {
        struct sockaddr_in serveraddr;
        memset(&serveraddr, 0, sizeof(serveraddr));

        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port   = htons(port);

        inet_pton(AF_INET, listenOnAddr, &serveraddr.sin_addr);

        if (bind(getSocket(),(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
        {
            lastError = "bind() failed";
            closeSocket();
            return false;
        }
    }
    else
    {
        struct sockaddr_in6 serveraddr;
        memset(&serveraddr, 0, sizeof(serveraddr));

        serveraddr.sin6_family = AF_INET6;
        serveraddr.sin6_port   = htons(port);

        inet_pton(AF_INET6, listenOnAddr, &serveraddr.sin6_addr);

        if (bind(getSocket(),(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
        {
            lastError = "bind() failed";
            closeSocket();
            return false;
        }
    }

    if (listen(getSocket(), backlog) < 0)
    {
        lastError = "listen() failed";
        closeSocket();
        return false;
    }

    return true;
}

bool Socket_TCP::PostAcceptSubInitialization()
{
	return true;
}

bool Socket_TCP::PostConnectSubInitialization()
{
	return true;
}
