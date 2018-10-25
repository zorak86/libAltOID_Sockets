#include "socket_unix.h"

#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#ifdef _WIN32
#error Unix sockets only works on unix :)
#else
#include <sys/un.h>
#include <sys/socket.h>
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////                        UNIX                                    /////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

Socket_UNIX::Socket_UNIX()
{
	socketType = STREAM_SOCKET;
}


bool Socket_UNIX::listenOn(uint16_t port, const char * listenOnAddr)
{
    if (isValidSocket()) closeSocket(); // close first

   // use the addr as path.
   sockaddr_un server_address;
   int         server_len;

   unlink(listenOnAddr);

   setSocket(socket(AF_UNIX, SOCK_STREAM, 0));
   if (!isValidSocket())
   {
      lastError = "socket() failed";
      return false;
   }

   server_address.sun_family = AF_UNIX;
   strncpy(server_address.sun_path, listenOnAddr, sizeof(server_address.sun_path)-1);
   server_len = sizeof(server_address);

   if (bind(getSocket(),(struct sockaddr *)&server_address,server_len) < 0)
   {
       lastError = "bind() failed";
       closeSocket();
       return false;
   }
   if (listen(getSocket(), 10) < 0)
   {
       lastError = "bind() failed";
       closeSocket();
       return false;
   }

   return true;
}

bool Socket_UNIX::connectTo(const char * hostname, uint16_t port, uint32_t timeout)
{
    if (isValidSocket()) closeSocket(); // close first

    int         len;
    sockaddr_un address;

    setSocket( socket(AF_UNIX, SOCK_STREAM, 0) );
    if (!isValidSocket())
    {
        lastError = "socket() failed";
        return false;
    }

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, hostname);
    len = sizeof(address);

    // Set the timeout here.
    setReadTimeout(timeout);

    if(connect(getSocket(), (sockaddr*)&address, len) == -1)
    {
        lastError = "socket() failed";
        return false;
    }

    return true;
}

Stream_Socket * Socket_UNIX::acceptConnection()
{
    int sdconn;

    Stream_Socket * cursocket = nullptr;

    if ((sdconn = accept(getSocket(), nullptr, nullptr)) >= 0)
    {
        cursocket = new Stream_Socket;
        // Set the proper socket-
        cursocket->setSocket(sdconn);
    }
    else
    {
        // Establish the error.
        lastError = "accept() failed";
    }

    // return the socket class.
    return cursocket;
}
