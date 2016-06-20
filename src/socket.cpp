#include "socket.h"

#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>

#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif
#include <alt_mutex/locker_mutex.h>

Mutex_Instance Socket::sockMutex;
bool Socket::socketInitialized = false;
bool Socket::badSocket = false;

Socket::Socket()
{
    socketSystemInitialization();

	socketType = UNINITIALIZED_SOCKET;
	useWrite = false;
	lastError = "";
    // Create the socket...
    microSocket.reset(new MicroSocket);
    memset(remotePair, 0, sizeof(remotePair));
}

Socket::Socket(int sockfd)
{
    Socket();
    setSocket(sockfd);
}

Socket::~Socket()
{

}

void Socket::setUseWrite()
{
	// prevent the application from crash, ignore the sigpipes:
#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif
	// use write/read functions instead send/recv
	useWrite = true;
}

void Socket::setRecvBuffer(int buffsize)
{
    if (!(*microSocket).IsValidSocket()) return;
#ifdef _WIN32
    setsockopt((*microSocket).socket, SOL_SOCKET, SO_RCVBUF, (char *) &buffsize, sizeof(buffsize));
#else
    setsockopt((*microSocket).socket, SOL_SOCKET, SO_RCVBUF, &buffsize, sizeof(buffsize));
#endif
}

bool Socket::isConnected()
{
	return false;
}

bool Socket::connectTo(const char *, uint16_t, uint32_t)
{
	return false;
}

void Socket::tryConnect(const char *hostname, const uint16_t port,
        uint32_t timeout)
{
	while (!connectTo(hostname, port, timeout))
	{
		// Try to reconnect if fail...
	}
}

bool Socket::listenOn(uint16_t, const char *)
{
    return false;
}

void Socket::detachSocket()
{
    (*microSocket).socket = -1;
}

int Socket::closeSocket()
{
    if (!(*microSocket).IsValidSocket()) return 0;

    int i = close((*microSocket).socket);
    (*microSocket).socket = -1;
    return i;
}

const char * Socket::getLastError() const
{
	return lastError;
}

void Socket::setSocket(int _sockfd)
{
    if (getSocket() != -1)
	{
		// PUT YOUR WARN ABOUT REWRITE:
	}
    (*microSocket).socket = _sockfd;
}

uint16_t Socket::getPort()
{
    if (!(*microSocket).IsValidSocket()) return 0;

	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
    if (getsockname((*microSocket).socket, (struct sockaddr *) &sin, &len) == -1)
	{
		lastError = "Error resolving port";
		return 0;
	}
	return ntohs(sin.sin_port);
}

int Socket::partialRead(void *data, uint32_t datalen)
{
    if (!(*microSocket).IsValidSocket()) return -1;
    if (!datalen) return 0;
    if (!useWrite)
	{
        ssize_t recvLen = recv((*microSocket).socket, (char *) data, datalen, 0);
		return recvLen;
	}
	else
	{
        ssize_t recvLen = read((*microSocket).socket, (char *) data, datalen);
		return recvLen;
    }
}

int Socket::partialWrite(void *data, uint32_t datalen)
{
    if (!(*microSocket).IsValidSocket()) return -1;
    if (!datalen) return 0;
    if (!useWrite)
    {
#ifdef WIN32
        ssize_t sendLen = send((*microSocket).socket, (char *) data, datalen, 0);
#else
        ssize_t sendLen = send((*microSocket).socket, (char *) data, datalen, MSG_NOSIGNAL);
#endif
        return sendLen;
    }
    else
    {
        ssize_t sendLen = write((*microSocket).socket, (char *) data, datalen);
        return sendLen;
    }
}

void Socket::socketSystemInitialization()
{
    Locker_Mutex lm(&sockMutex);
    if (!socketInitialized)
    {
#ifdef _WIN32
        int wsaerr;

        WORD wVersionRequested;
        WSADATA wsaData;

        wVersionRequested = MAKEWORD(2, 2);
        wsaerr = WSAStartup(wVersionRequested, &wsaData);
        if (wsaerr != 0)
        {
            // dll not found.
            badSocket = true;
            return;
        }

        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2 )
        {
            // not supported.
            WSACleanup();
            badSocket = true;
            return;
        }
#endif
        socketInitialized = true;
    }
}

unsigned short Socket::getRemotePort() const
{
    return remotePort;
}

void Socket::setRemotePort(unsigned short value)
{
    remotePort = value;
}

bool Socket::setReadTimeout(unsigned int _timeout)
{
    if (!(*microSocket).IsValidSocket()) return false;

    struct timeval timeout;
    timeout.tv_sec = _timeout;
	timeout.tv_usec = 0;

#ifdef _WIN32
    if ((setsockopt((*microSocket).socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout))) == -1)
#else
    if ((setsockopt((*microSocket).socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) == -1)
#endif
	{
		return false;
	}
	return true;
}

bool Socket::isValidSocket() const
{
    return (*microSocket).IsValidSocket();
}

int Socket::getSocket() const
{
    return (*microSocket).socket;
}

void Socket::getRemotePair(char * address) const
{
	strncpy(address, remotePair, 64);
}

void Socket::setRemotePair(const char * address)
{
	strncpy(remotePair, address, 64);
}

int Socket::shutdownSocket()
{
    if (!(*microSocket).IsValidSocket()) return -1;

    return shutdown((*microSocket).socket, 2);
}
