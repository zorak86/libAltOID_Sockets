#ifndef STREAM_PIPE_H
#define STREAM_PIPE_H

#include "stream_socket.h"

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
    bool StartThreaded(bool _autoDelete = true);
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
     * @brief setAutoDelete Auto Delete the pipe object when finish his job
     * @param value true for autodelete (default), false for not.
     */
    void setAutoDelete(bool value = true);
    /**
     * @brief shutdownRemotePeer set to shutdown remote socket peer on finish.
     * @param value true for close the remote peer (default), false for not.
     */
    void setToShutdownRemotePeer(bool value = true);
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
    bool getAutoDelete() const;
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

private:
    Stream_Socket * socket_peers[2];

    std::atomic<uint64_t> sentBytes,recvBytes;

    std::atomic<int> finishingPeer;
    std::atomic<unsigned int> blockSize;

    std::atomic<bool> shutdownRemotePeerOnFinish;
    bool autoDelete;
    bool autoDeleteSocketsOnExit;
};

#endif // STREAM_PIPE_H
