#include "stream_pipe_thread_base.h"

Stream_Pipe_Thread_Base::Stream_Pipe_Thread_Base(Stream_Socket *src, Stream_Socket *dst)
{
    this->src = src;
    this->dst = dst;
    block_fwd = NULL;
    block_rev = NULL;
    setBlockSize(8192);
}

Stream_Pipe_Thread_Base::~Stream_Pipe_Thread_Base()
{
    delete [] block_fwd;
    delete [] block_rev;
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
    if (block_fwd) delete [] block_fwd;
    if (block_rev) delete [] block_rev;
    blockSize = value;
    block_fwd = new char[blockSize];
    block_rev = new char[blockSize];
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
    char * curBlock = fwd?block_fwd:block_rev;

    int bytesReceived;
    if ((bytesReceived=src1->partialRead(curBlock,blockSize))>0)
    {
        if (!writeBlock(curBlock,bytesReceived,fwd)) return -2;
        // Update Counters:
        return bytesReceived;
    }
    return -1;
}

int Stream_Pipe_Thread_Base::partialRead(void *data, uint32_t datalen, bool fwd)
{
    return (fwd?src:dst)->partialRead(data,datalen);
}
