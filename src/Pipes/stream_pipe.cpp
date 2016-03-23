#include "stream_pipe.h"

Stream_Pipe::Stream_Pipe()
{
    sentBytes = 0;
    recvBytes = 0;

    socket_peers[0] = NULL;
    socket_peers[1] = NULL;

    finishingPeer = -1;
    autoDeleteSocketsOnExit = false;

    setBlockSize();
    setAutoDelete();
    setToShutdownRemotePeer();
}

Stream_Pipe::~Stream_Pipe()
{
    if (autoDeleteSocketsOnExit)
    {
        if (socket_peers[0]) delete socket_peers[0];
        if (socket_peers[1]) delete socket_peers[1];
    }
}

void * pipeThread(void * _stp)
{
    Stream_Pipe * stp = (Stream_Pipe *)_stp;
    stp->StartBlocking();
    if (stp->getAutoDelete())
    {
        delete stp;
    }
    pthread_exit(NULL);
    return NULL;
}

bool Stream_Pipe::StartThreaded(bool _autoDelete)
{
    if (!socket_peers[0] || !socket_peers[1]) return false;

    autoDelete = _autoDelete;

    pthread_t pipeThreadP;
    pthread_create(&pipeThreadP, NULL, pipeThread, this);
    pthread_detach(pipeThreadP);

    return true;
}

void * remotePeerThread(void * _stp)
{
    Stream_Pipe * stp = (Stream_Pipe *)_stp;
    stp->StartPeerBlocking(1);
    pthread_exit(NULL);
    return NULL;
}

int Stream_Pipe::StartBlocking()
{
    if (!socket_peers[0] || !socket_peers[1]) return -1;

    pthread_t remotePeerThreadP;
    pthread_create(&remotePeerThreadP, NULL, remotePeerThread, this);
    StartPeerBlocking(0);
    pthread_join(remotePeerThreadP,NULL);

    // All connections terminated.
    if (shutdownRemotePeerOnFinish)
    {
        // close them also.
        socket_peers[1]->closeSocket();
        socket_peers[0]->closeSocket();
    }

    return finishingPeer;
}

bool Stream_Pipe::StartPeerBlocking(unsigned char i)
{
    if (i>1) return false;

    unsigned char nextpeer = i==0?1:0;

    Stream_Socket * current = socket_peers[i];
    Stream_Socket * next = socket_peers[nextpeer];

    int bytesReceived;
    char * block = new char[blockSize];

    while ((bytesReceived=current->partialRead(block,blockSize))>0)
    {
        if (!next->writeBlock(block,bytesReceived))
        {
            socket_peers[nextpeer]->shutdownSocket();
            finishingPeer = nextpeer;
            return true;
        }

        // Update Counters:
        if (i==0) sentBytes+= bytesReceived;
        else recvBytes += bytesReceived;
    }

    if (shutdownRemotePeerOnFinish)
    {
        socket_peers[nextpeer]->shutdownSocket();
    }

    finishingPeer = i;
    return true;
}

bool Stream_Pipe::SetPeer(unsigned char i, Stream_Socket *s)
{
    if (i>1) return false;
    socket_peers[i] = s;
    return true;
}

Stream_Socket *Stream_Pipe::GetPeer(unsigned char i)
{
    if (i>1) return NULL;
    return socket_peers[i];
}

void Stream_Pipe::setAutoDelete(bool value)
{
    autoDelete = value;
}

void Stream_Pipe::setToShutdownRemotePeer(bool value)
{
    shutdownRemotePeerOnFinish = value;
}

void Stream_Pipe::setBlockSize(unsigned int value)
{
    blockSize = value;
}

uint64_t Stream_Pipe::getSentBytes() const
{
    return sentBytes;
}

uint64_t Stream_Pipe::getRecvBytes() const
{
    return recvBytes;
}

bool Stream_Pipe::getAutoDelete() const
{
    return autoDelete;
}

bool Stream_Pipe::getAutoDeleteSocketsOnExit() const
{
    return autoDeleteSocketsOnExit;
}

void Stream_Pipe::setAutoDeleteSocketsOnExit(bool value)
{
    autoDeleteSocketsOnExit = value;
}