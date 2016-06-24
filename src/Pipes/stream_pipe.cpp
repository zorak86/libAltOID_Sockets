#include "stream_pipe.h"

void * pipeThread(void * _stp)
{
    Stream_Pipe * stp = (Stream_Pipe *)_stp;
    stp->StartBlocking();
    if (stp->getAutoDeleteStreamPipeOnThreadExit())
    {
        delete stp;
    }
    pthread_exit(NULL);
    return NULL;
}

Stream_Pipe::Stream_Pipe()
{
    customPipeProcessor = NULL;
    sentBytes = 0;
    recvBytes = 0;

    socket_peers[0] = NULL;
    socket_peers[1] = NULL;

    finishingPeer = -1;
    autoDeleteSocketsOnExit = false;

    setAutoDeleteStreamPipeOnThreadExit(true);
    setToShutdownRemotePeer(true);
    setToCloseRemotePeer(true);
}

Stream_Pipe::~Stream_Pipe()
{
    if (autoDeleteSocketsOnExit)
    {
        if (socket_peers[0]) delete socket_peers[0];
        if (socket_peers[1]) delete socket_peers[1];
    }
}

bool Stream_Pipe::StartThreaded(bool _autoDeleteStreamPipeOnExit, bool detach)
{
    if (!socket_peers[0] || !socket_peers[1]) return false;

    autoDeleteStreamPipeOnExit = _autoDeleteStreamPipeOnExit;

    pthread_create(&pipeThreadP, NULL, pipeThread, this);

    if (autoDeleteStreamPipeOnExit || detach)
        pthread_detach(pipeThreadP);

    return true;
}

int Stream_Pipe::WaitForThread()
{
    pthread_join(pipeThreadP,NULL);
    return finishingPeer;
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
    if (closeRemotePeerOnFinish)
    {
        // close them also.
        socket_peers[1]->closeSocket();
        socket_peers[0]->closeSocket();
    }

    return finishingPeer;
}

bool Stream_Pipe::StartPeerBlocking(unsigned char cur)
{
    if (cur>1) return false;

    unsigned char next = cur==0?1:0;
    std::atomic<uint64_t> * bytesCounter = cur==0?&sentBytes:&recvBytes;

    if (!customPipeProcessor)
        customPipeProcessor = new Stream_Pipe_Thread_Base(socket_peers[cur],socket_peers[next]);

    int dataRecv=0;
    while (dataRecv>=0)
    {
        dataRecv = cur==0?customPipeProcessor->processPipeFWD():customPipeProcessor->processPipeREV();

        if (dataRecv>=0) *bytesCounter+=dataRecv;
        else if (dataRecv==-1 && shutdownRemotePeerOnFinish)
        {
            socket_peers[next]->shutdownSocket();
            finishingPeer = cur;
        }
    }

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

void Stream_Pipe::setAutoDeleteStreamPipeOnThreadExit(bool value)
{
    autoDeleteStreamPipeOnExit = value;
}

void Stream_Pipe::setToShutdownRemotePeer(bool value)
{
    shutdownRemotePeerOnFinish = value;
}

void Stream_Pipe::setToCloseRemotePeer(bool value)
{
    closeRemotePeerOnFinish = value;
}

uint64_t Stream_Pipe::getSentBytes() const
{
    return sentBytes;
}

uint64_t Stream_Pipe::getRecvBytes() const
{
    return recvBytes;
}

bool Stream_Pipe::getAutoDeleteStreamPipeOnThreadExit() const
{
    return autoDeleteStreamPipeOnExit;
}

bool Stream_Pipe::getAutoDeleteSocketsOnExit() const
{
    return autoDeleteSocketsOnExit;
}

void Stream_Pipe::setAutoDeleteSocketsOnExit(bool value)
{
    autoDeleteSocketsOnExit = value;
}


void Stream_Pipe::setCustomPipeProcessor(Stream_Pipe_Thread_Base *value)
{
    customPipeProcessor = value;
}
