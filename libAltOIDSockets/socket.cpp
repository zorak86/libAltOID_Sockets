#include "../include/alt_sockets/socket.h"

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
	sockfd = -1;
	useWrite = false;
	lastError = "";
	memset(remotePair, 0, sizeof(remotePair));
}

Socket::~Socket()
{
}

Socket::Socket(const int &_sockfd)
{
	socketType = UNINITIALIZED_SOCKET;
	useWrite = false;
	sockfd = _sockfd;
	lastError = "";
	// TODO: maybe we should obtain the remote pair from this socket file descriptor.
	memset(remotePair, 0, sizeof(remotePair));
}

Socket & Socket::operator=(const int & _sockfd)
{
	useWrite = false;
	sockfd = _sockfd;
	return *this;
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
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &buffsize, sizeof(buffsize));
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

char * Socket::getLastError() const
{
	return lastError;
}

void Socket::setSocket(int _sockfd)
{
	if (sockfd != -1)
	{
		// PUT YOUR WARN ABOUT REWRITE:
	}
	sockfd = _sockfd;
}

uint16_t Socket::getPort()
{
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(sockfd, (struct sockaddr *) &sin, &len) == -1)
	{
		lastError = "Error resolving port";
		return 0;
	}
	return ntohs(sin.sin_port);
}

int Socket::partialRead(void *data, uint32_t datalen)
{
	if (!datalen)
		return -1;
	if (!useWrite)
	{
		ssize_t recvLen = recv(sockfd, (char *) data, datalen, 0);
		return recvLen;
	}
	else
	{
		ssize_t recvLen = read(sockfd, (char *) data, datalen);
		return recvLen;
	}
}

bool Socket::setReadTimeout(unsigned int _timeout)
{
	struct timeval timeout;
	timeout.tv_sec = _timeout;
	timeout.tv_usec = 0;
	if ((setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
	        == -1)
	{
		return false;
	}
	return true;
}

bool Socket::isSocketSettedUp() const
{
	return (sockfd != -1);
}

int Socket::getSocket() const
{
	return sockfd;
}

void Socket::getRemotePair(char * address) const
{
	strncpy(address, remotePair, 64);
}

void Socket::setRemotePair(const char * address)
{
	strncpy(remotePair, address, 64);
}

void Socket::closeSocket()
{
	if (sockfd <= 0)
		return;
	close(sockfd);
	sockfd = -1;
}

int Socket::shutdownSocket()
{
	return shutdown(sockfd, 2);
}
