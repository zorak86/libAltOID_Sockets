#include "../include/alt_sockets/socket_tcp.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////                        TCP                                     /////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

Socket_TCP::Socket_TCP()
{
}

bool Socket_TCP::isConnected()
{
    if (sockfd<=0) return false;

    struct sockaddr peer;
    socklen_t peer_len;
    peer_len = sizeof(peer);
    if (getpeername(sockfd, &peer, &peer_len) == -1)
    {
        return false;
        sockfd = -1;
    }
    return true;
}

void Socket_TCP::setFlag_TCP_NODELAY()
{
    int flag = 1;
    setsockopt(sockfd,IPPROTO_TCP, TCP_NODELAY,(char *) &flag, sizeof(int));
}


bool Socket_TCP::connectTo(const char * hostname, uint16_t port, uint32_t timeout)
{
    if (sockfd) closeSocket(); // close first

    char servport[32];
    int rc;
    struct in6_addr serveraddr;
    struct addrinfo hints, *res=NULL;

    memset(&hints, 0x00, sizeof(hints));
    hints.ai_flags    = AI_NUMERICSERV;
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
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
        {
            lastError = "socket() failed";
            break;
        }

        // Set the read timeout here. (to zero)
        setReadTimeout(0);

        if (internalConnect(sockfd,resiter->ai_addr, resiter->ai_addrlen,timeout))
        {
            connected = true;
            break;
        }
        else
        {
            // drop the current socket... (and free the resource :))
            shutdown(sockfd,SHUT_RDWR);
            close(sockfd);
            sockfd=-1;
        }
    }

    freeaddrinfo(res);

    if (!connected)
    {
        lastError = "connect() failed";
        return false;
    }

    return true;
}

Stream_Socket Socket_TCP::acceptConnection()
{
    int sdconn;
    Stream_Socket cursocket;
    cursocket.setSocket(0);

    int32_t clilen;
    struct sockaddr_in cli_addr;
    clilen = sizeof(cli_addr);

    if ((sdconn = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen)) >= 0)
    {
        // Set the proper socket-
        cursocket.setSocket(sdconn);
        char ipAddr[80];
        inet_ntop(AF_INET, &cli_addr.sin_addr, ipAddr, sizeof(ipAddr));
        cursocket.setRemotePair(ipAddr);
    }
    // Establish the error.
    else lastError = "accept() failed";

    // return the socket class.
    return cursocket;
}

bool Socket_TCP::internalConnect(int sockfd, const sockaddr *addr, socklen_t addrlen, uint32_t timeout)
{
    int res2,valopt;

    // Non-blocking connect with timeout...
    if (!internalPassToNonBlocking(sockfd)) return false;

    // Trying to connect with timeout.
    res2 = connect(sockfd, addr, addrlen);
    if (res2 < 0)
    {
        if (errno == EINPROGRESS)
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
                if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0)
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
}

bool Socket_TCP::internalPassToNonBlocking(int sockfd)
{
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
}

bool Socket_TCP::listenOn(uint16_t port, const char * listenOnAddr, bool useIPv4, int recvbuffer,unsigned int backlog)
{
    int on=1;

    if (useIPv4)
    {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            lastError = "socket() failed";
            return false;
        }
    }
    else
    {
        if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
        {
            lastError = "socket() failed";
            return false;
        }
    }

    if (recvbuffer) setRecvBuffer(recvbuffer);

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on,sizeof(on)) < 0)
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

        if (bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
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

        if (bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
        {
            lastError = "bind() failed";
            closeSocket();
            return false;
        }
    }

    if (listen(sockfd, backlog) < 0)
    {
        lastError = "bind() failed";
        closeSocket();
        return false;
    }

    return true;
}
