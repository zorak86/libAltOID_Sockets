#ifndef TESTX_H
#define TESTX_H

#include <pthread.h>

#include "socket_tcp.h"

/**
 * Class for managing the client on his thread.
 */
class Threaded_Client_Control
{
public:
	/**
	 * constructor
	 */
    Threaded_Client_Control();
    /**
     * destructor
     */
    ~Threaded_Client_Control();
    /**
     * Start the thread of the client.
     */
    void Start();
    /**
     * Kill the client socket
     */
    void StopSocket();
    /**
     * Wait until the socket thread ends.
     */
  //  void Join();
    /**
     * Set parent (stream acceptor object)
     * @param parent parent
     */
    void SetParent(void * parent);
    /**
     * Get Parent (stream acceptor object)
     * @return parent
     */
    void * GetParent();
    /**
     * Set callback used for
     */
    void SetCallbackFunction(bool (*_CallbackFunction)(void *, Stream_Socket *), void *obj);
    /**
     * Call callback
     * to be used from the client thread.
     */
    void CallCallback();
    /**
     * Set socket
     */
    void setSocket(Stream_Socket * _clientSocket);

private:
    pthread_t clientThread;
    Stream_Socket * clientSocket;
    bool (*CallbackFunction)(void *,Stream_Socket *);
    void * obj;
    void * parent;
};

#endif // TESTX_H
