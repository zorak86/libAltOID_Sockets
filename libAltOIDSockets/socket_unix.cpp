#include "../include/alt_sockets/socket_unix.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////                        UNIX                                    /////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

Socket_UNIX::Socket_UNIX()
{
	socketType = STREAM_SOCKET;
}

bool Socket_UNIX::isConnected()
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

bool Socket_UNIX::listenOn(uint16_t port, const char * listenOnAddr)
{
   // use the addr as path.
   sockaddr_un server_address;
   int         server_len;

   unlink(listenOnAddr);

   if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
   {
      lastError = "socket() failed";
      return false;
   }

   server_address.sun_family = AF_UNIX;
   strncpy(server_address.sun_path, listenOnAddr, sizeof(server_address.sun_path));
   server_len = sizeof(server_address);

   if (bind(sockfd,(struct sockaddr *)&server_address,server_len) < 0)
   {
       lastError = "bind() failed";
       closeSocket();
       return false;
   }
   if (listen(sockfd, 10) < 0)
   {
       lastError = "bind() failed";
       closeSocket();
       return false;
   }

   return true;
}

bool Socket_UNIX::connectTo(const char * hostname, uint16_t port, uint32_t timeout)
{
    if (sockfd) closeSocket(); // close first

    int         len;
    sockaddr_un address;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        lastError = "socket() failed";
        return false;
    }

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, hostname);
    len = sizeof(address);

    // Set the timeout here.
    setReadTimeout(timeout);

    if(connect(sockfd, (sockaddr*)&address, len) == -1)
    {
        lastError = "socket() failed";
        return false;
    }

    return true;
}

Stream_Socket Socket_UNIX::acceptConnection()
{
    int sdconn;
    Stream_Socket cursocket;
    cursocket.setSocket(0);
    if ((sdconn = accept(sockfd, NULL, NULL)) >= 0)
    {
        // Set the proper socket-
        cursocket.setSocket(sdconn);
    }
    // Establish the error.
    else lastError = "accept() failed";

    // return the socket class.
    return cursocket;
}
