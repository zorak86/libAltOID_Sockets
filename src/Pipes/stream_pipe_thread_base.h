#ifndef STREAM_PIPE_THREAD_BASE_H
#define STREAM_PIPE_THREAD_BASE_H

#include "stream_socket.h"
#include <alt_mutex/locker_mutex.h>
#include <atomic>


class Stream_Pipe_Thread_Base
{
public:
    Stream_Pipe_Thread_Base();
    virtual ~Stream_Pipe_Thread_Base();

    void setStreamSockets(Stream_Socket * src, Stream_Socket * dst);

    /**
     * @brief processPipeFWD reads from SRC and write into DST making the proper transformations
     * @param src socket to read from.
     * @param dst socket to write into.
     * @return -1 if src terminated the connection, -2 if dst terminated the connection, otherwise, bytes processed.
     */
    virtual int processPipeFWD();
    /**
     * @brief processPipeREV
     * @param src socket to read from.
     * @param dst socket to write into.
     * @return -1 if src terminated the connection, -2 if dst terminated the connection, otherwise, bytes processed.
     */
    virtual int processPipeREV();


    virtual bool startPipeSync();

    /**
     * @brief setBlockSize Set Transfer Block Chunk Size
     * @param value Chunk size, default 8192
     */
    void setBlockSize(unsigned int value = 8192);
    /**
     * @brief writeBlock write a whole data into next socket
     * @param data data to be written
     * @param datalen data lenght to be written
     * @return true if success
     */
    bool writeBlockL(const void *data, uint32_t datalen, bool fwd = true);

protected:
    Stream_Socket * src;
    char * block_fwd;
    std::atomic<unsigned int> blockSize;
    int partialReadL(void *data, uint32_t datalen, bool fwd = true);

private:
    /**
     * @brief simpleProcessPipe simple pipe processor (data in, data out as is)
     * @param src socket to read from.
     * @param dst socket to read from.
     * @return -1 if src terminated the connection, -2 if dst terminated the connection, otherwise, bytes processed.
     */
    int simpleProcessPipe(bool fwd);


    Stream_Socket * dst;
    char * block_rev;

    Mutex_Instance mt_fwd, mt_rev;
};

#endif // STREAM_PIPE_THREAD_BASE_H
