/*
 * stream_socket.cpp
 *
 *  Created on: Aug 23, 2015
 *      Author: zorak
 */

#include "alt_sockets/stream_socket.h"

#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

Stream_Socket::Stream_Socket()
{
	socketType = STREAM_SOCKET;
}

Stream_Socket::~Stream_Socket()
{
}


bool Stream_Socket::writeBlock(const char *data)
{
    return writeBlock(data,strlen(data)+1);
}

bool Stream_Socket::writeBlock(const void *data, uint32_t datalen)
{
    if (!useWrite)
    {
        int32_t sent_bytes = 0;
        int32_t left_to_send = datalen;

        // Send the raw data.
        // datalen-left_to_send is the _size_ of the data already sent.
        while (left_to_send && (sent_bytes = send(sockfd, (char *) data + (datalen - left_to_send), left_to_send>4096?4096:left_to_send, MSG_NOSIGNAL)) <= left_to_send)
        {
            if (sent_bytes == -1)
            {
                // Error sending data. (returns false.)
                shutdown(sockfd, 2);
                return false;
            }
            // Substract the data that was already sent from the count.
            else
                left_to_send -= sent_bytes;
        }

        // Failed to achieve sending the contect on 5 attempts
        if (left_to_send != 0)
        {
            // left_to_send must always return 0 bytes. otherwise here we have an error (return false)
            shutdown(sockfd, 2);
            return false;
        }
        return true;
    }
    else
    {
        int32_t sent_bytes = 0;
        int32_t left_to_send = datalen;

        // Send the raw data.
        // datalen-left_to_send is the _size_ of the data already sent.
        while (left_to_send && (sent_bytes = write(sockfd, (char *) data + (datalen - left_to_send), left_to_send>4096?4096:left_to_send)) <= left_to_send)
        {
            if (sent_bytes == -1)
            {
                // Error sending data. (returns false.)
                shutdown(sockfd, 2);
                return false;
            }
            // Substract the data that was already sent from the count.
            else
                left_to_send -= sent_bytes;
        }

        // Failed to achieve sending the contect on 5 attempts
        if (left_to_send != 0)
        {
            // left_to_send must always return 0 bytes. otherwise here we have an error (return false)
            shutdown(sockfd, 2);
            return false;
        }
        return true;
    }
}

Stream_Socket Stream_Socket::acceptConnection()
{
	Stream_Socket x;
	return x;
}

bool Stream_Socket::readBlock(void *data, uint32_t datalen)
{
    if (!useWrite)
    {
        int total_recv_bytes = 0;
        int local_recv_bytes = 0;

        if (datalen<=0) return true;

        // Try to receive the maximum amount of data left.
        while ( (datalen - total_recv_bytes)>0 // there are bytes to read.
                && (local_recv_bytes = recv(sockfd, ((char *) data) + total_recv_bytes, datalen - total_recv_bytes, 0)) >0 // receive bytes. if error, will return with -1.
                )
        {
            // Count the data received.
            total_recv_bytes += local_recv_bytes;
        }

        if ((unsigned int)total_recv_bytes<datalen)
        {
            // bad, we had to receive at least bytes_to_read...
            return false;
        }

        // Otherwise... return true.
        return true;
    }
    else
    {
        int total_recv_bytes = 0;
        int local_recv_bytes = 0;

        if (datalen<=0) return true;

        // Try to receive the maximum amount of data left.
        while ( (datalen - total_recv_bytes)>0 // there are bytes to read.
                && (local_recv_bytes = read(sockfd, ((char *) data) + total_recv_bytes, datalen - total_recv_bytes)) >0 // receive bytes. if error, will return with -1.
                )
        {
            // Count the data received.
            total_recv_bytes += local_recv_bytes;
        }

        if ((unsigned int)total_recv_bytes<datalen)
        {
            // bad, we had to receive at least bytes_to_read...
            return false;
        }

        // Otherwise... return true.
        return true;
    }
}

bool Stream_Socket::isConnected()
{
	return false;
}

bool Stream_Socket::listenOn(uint16_t port, const char* listenOnAddr,
        bool useIPv4, int recvbuffer, unsigned int backlog)
{
	return false;
}

bool Stream_Socket::connectTo(const char* hostname, uint16_t port,
        uint32_t timeout)
{
	return false;
}
