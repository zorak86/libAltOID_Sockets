#include "stream_pipe_thread_base.h"

Stream_Pipe_Thread_Base::Stream_Pipe_Thread_Base(Stream_Socket *src, Stream_Socket *dst)
{
    this->src = src;
    this->dst = dst;
    block = NULL;
    setBlockSize(8192);
}

Stream_Pipe_Thread_Base::~Stream_Pipe_Thread_Base()
{
    delete [] block;
}

int Stream_Pipe_Thread_Base::processPipeFWD()
{
    return simpleProcessPipe(true);
}

int Stream_Pipe_Thread_Base::processPipeREV()
{
    return simpleProcessPipe(false);
}

void Stream_Pipe_Thread_Base::setBlockSize(unsigned int value)
{
    if (block) delete [] block;
    blockSize = value;
    block = new char[blockSize];
}

bool Stream_Pipe_Thread_Base::writeBlock(const void *data, uint32_t datalen, bool fwd)
{
    Locker_Mutex lmt(fwd?&mt_fwd:&mt_rev);
    Stream_Socket *dstX=fwd?dst:src;
    return dstX->writeBlock(data,datalen);
}

int Stream_Pipe_Thread_Base::simpleProcessPipe(bool fwd)
{
    Stream_Socket *src1=fwd?src:dst;

    int bytesReceived;
    if ((bytesReceived=src1->partialRead(block,blockSize))>0)
    {
        if (!writeBlock(block,bytesReceived,fwd))
        {
            return -2;
        }
        // Update Counters:
        return bytesReceived;
    }
    return -1;
}

int Stream_Pipe_Thread_Base::partialRead(void *data, uint32_t datalen, bool fwd)
{
    return (fwd?src:dst)->partialRead(data,datalen);
}
