#ifndef STREAM_PIPE_H
#define STREAM_PIPE_H

#include "stream_socket.h"
#include <stdint.h>
#include <atomic>

/**
 * @brief The Stream_Pipe class connect two pipe sockets.
 */
class Stream_Pipe
{
public:
    /**
     * @brief Stream_Pipe constructor.
     */
    Stream_Pipe();
    /**
     * @brief Stream_Pipe destructor.
     */
    ~Stream_Pipe();
    /**
     * @brief Start, begin the communication between peers in threaded mode.
     * @param autoDelete true (default) if going to delete the whole pipe when finish.
     * @return true if initialized, false if not.
     */
    bool StartThreaded(bool _autoDeleteStreamPipeOnExit = true, bool detach = true);

    /**
     * @brief WaitForThread will block-wait until thread finishes
     * @return -1 failed, 0: socket 0 closed the connection, 1: socket 1 closed the connection.
     */
    int WaitForThread();

    /**
     * @brief StartBlocking, begin the communication between peers blocking until it ends.
     * @return -1 failed, 0: socket 0 closed the connection, 1: socket 1 closed the connection.
     */
    int StartBlocking();
    /**
     * @brief StartPeerBlocking, begin the communication between peer i to the next peer.
     * @param i peer number (0 or 1)
     * @return true if transmitted and closed, false if failed.
     */
    bool StartPeerBlocking(unsigned char i);
    /**
     * @brief SetPeer Set Stream Socket Peer (0 or 1)
     * @param i peer number: 0 or 1.
     * @param s peer established socket.
     * @return true if peer setted successfully.
     */
    bool SetPeer(unsigned char i, Stream_Socket * s);
    /**
     * @brief GetPeer Get the Pipe Peers
     * @param i peer number (0 or 1)
     * @return Stream Socket Peer.
     */
    Stream_Socket * GetPeer(unsigned char i);
    /**
     * @brief setAutoDelete Auto Delete the pipe object when finish threaded job.
     * @param value true for autodelete (default), false for not.
     */
    void setAutoDeleteStreamPipeOnThreadExit(bool value = true);
    /**
     * @brief shutdownRemotePeer set to shutdown both sockets peer on finish.
     * @param value true for close the remote peer (default), false for not.
     */
    void setToShutdownRemotePeer(bool value = true);
    /**
     * @brief closeRemotePeer set to close both sockets peer on finish.
     * @param value true for close the remote peer (default), false for not.
     */
    void setToCloseRemotePeer(bool value = true);
    /**
     * @brief setBlockSize Set Transfer Block Chunk Size
     * @param value Chunk size, default 8192
     */
    void setBlockSize(unsigned int value = 8192);
    /**
     * @brief getSentBytes Get bytes transmitted from peer 0 to peer 1.
     * @return bytes transmitted.
     */
    uint64_t getSentBytes() const;
    /**
     * @brief getRecvBytes Get bytes  transmitted from peer 1 to peer 0.
     * @return bytes transmitted.
     */
    uint64_t getRecvBytes() const;
    /**
     * @brief getAutoDelete Get if this class autodeletes when pipe is over.
     * @return true if autodelete is on.
     */
    bool getAutoDeleteStreamPipeOnThreadExit() const;
    /**
     * @brief getAutoDeleteSocketsOnExit Get if pipe endpoint sockets are going to be deleted when this class is destroyed.
     * @return true if it's going to be deleted.
     */
    bool getAutoDeleteSocketsOnExit() const;
    /**
     * @brief setAutoDeleteSocketsOnExit Set if pipe endpoint sockets are going to be deleted when this class is destroyed.
     * @param value true if you want sockets to be deleted on exit.
     */
    void setAutoDeleteSocketsOnExit(bool value);

    uint64_t getWayRecvBytes() const;
    uint64_t getWaySentBytes() const;

private:
    Stream_Socket * socket_peers[2];

    std::atomic<uint64_t> sentBytes,recvBytes, wayRecvBytes, waySentBytes;

    std::atomic<int> finishingPeer;
    std::atomic<unsigned int> blockSize;

    std::atomic<bool> shutdownRemotePeerOnFinish;
    std::atomic<bool> closeRemotePeerOnFinish;

    bool autoDeleteStreamPipeOnExit;
    bool autoDeleteSocketsOnExit;


    pthread_t pipeThreadP;
};

#endif // STREAM_PIPE_H
