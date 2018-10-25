#include "stream_pipe_thread_base.h"

Stream_Pipe_Thread_Base::Stream_Pipe_Thread_Base()
{
    block_fwd = nullptr;
    block_rev = nullptr;
    setBlockSize(8192);
}

Stream_Pipe_Thread_Base::~Stream_Pipe_Thread_Base()
{
    delete [] block_fwd;
    delete [] block_rev;
}

void Stream_Pipe_Thread_Base::setStreamSockets(Stream_Socket *src, Stream_Socket *dst)
{
    this->src = src;
    this->dst = dst;
}

int Stream_Pipe_Thread_Base::processPipeFWD()
{
    return simpleProcessPipe(true);
}

int Stream_Pipe_Thread_Base::processPipeREV()
{
    return simpleProcessPipe(false);
}

bool Stream_Pipe_Thread_Base::startPipeSync()
{
    return true;
}

void Stream_Pipe_Thread_Base::setBlockSize(unsigned int value)
{
    if (block_fwd) delete [] block_fwd;
    if (block_rev) delete [] block_rev;
    blockSize = value;
    block_fwd = new char[blockSize];
    block_rev = new char[blockSize];
}

bool Stream_Pipe_Thread_Base::writeBlockL(const void *data, uint32_t datalen, bool fwd)
{
    Locker_Mutex lmt(fwd?&mt_fwd:&mt_rev);
    Stream_Socket *dstX=fwd?dst:src;
    return dstX->writeBlock(data,datalen);
}

int Stream_Pipe_Thread_Base::simpleProcessPipe(bool fwd)
{
    char * curBlock = fwd?block_fwd:block_rev;

    int bytesReceived;
    if ((bytesReceived=partialReadL(curBlock,blockSize,fwd))>0)
    {
        if (!writeBlockL(curBlock,bytesReceived,fwd)) return -2;
        // Update Counters:
        return bytesReceived;
    }
    return -1;
}

int Stream_Pipe_Thread_Base::partialReadL(void *data, uint32_t datalen, bool fwd)
{
    return (fwd?src:dst)->partialRead(data,datalen);
}
