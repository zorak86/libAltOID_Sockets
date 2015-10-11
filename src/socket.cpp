#include "socket.h"

#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>


Socket::Socket()
{
	socketType = UNINITIALIZED_SOCKET;
	useWrite = false;
	lastError = "";
    // Create the socket...
    microSocket.reset(new MicroSocket);
	memset(remotePair, 0, sizeof(remotePair));
}

Socket::~Socket()
{

}

void Socket::setUseWrite()
{
	// prevent the application from crash, ignore the sigpipes:
	signal(SIGPIPE, SIG_IGN);
	// use write/read functions instead send/recv
	useWrite = true;
}

void Socket::setRecvBuffer(int buffsize)
{
    if (!(*microSocket).IsValidSocket()) return;
    setsockopt((*microSocket).socket, SOL_SOCKET, SO_RCVBUF, &buffsize, sizeof(buffsize));
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

char * Socket::getLastError() const
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
        ssize_t sendLen = send((*microSocket).socket, (char *) data, datalen, 0);
        return sendLen;
    }
    else
    {
        ssize_t sendLen = write((*microSocket).socket, (char *) data, datalen);
        return sendLen;
    }
}

bool Socket::setReadTimeout(unsigned int _timeout)
{
    if (!(*microSocket).IsValidSocket()) return false;

	struct timeval timeout;
	timeout.tv_sec = _timeout;
	timeout.tv_usec = 0;
    if ((setsockopt((*microSocket).socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) == -1)
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
