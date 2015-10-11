#ifndef THREADEDACCEPTEDCONTROL_H
#define THREADEDACCEPTEDCONTROL_H

#include <list>

#include "stream_socket.h"
#include "alt_mutex/mutex_instance.h"
#include "alt_mutex/base_mutex_rw.h"

#include "threaded_client_control.h"

/**
 * Thread generator class for accepting stream based connections.
 */
class Threaded_Stream_Acceptor : public Base_Mutex_RW
{
public:
	/**
	 * Constructor
	 */
    Threaded_Stream_Acceptor();
    /**
     * Destructor
     * WARN: when you finalize this class, the listening socket is closed. please open another one (don't reuse it)
     */
    ~Threaded_Stream_Acceptor();

    /**
     * start accepting connections from the socket in a new thread.
     */
    bool start();

    // check mt here.
    void stop();

    /**
     * set callback function for managing the client newly generated socket
     * @param _CallbackFunction function that receives the new connection
     * @param obj Object to pass as a first argument
     */
    void setCallbackFunction(bool (*_CallbackFunction)(void *, Stream_Socket *), void * obj);
    /**
     * Set the socket that will be used to accept new clients.
     * WARNING: acceptorSocket will be deleted when this class finishes.
     */
    void setAcceptorSocket(Stream_Socket *acceptorSocket);

    /**
     * Do accept on the acceptor socket.
     * @return true if we can still accept a new connection
     */
    bool acceptClient();

    /**
     * Finalize/Catch the client thread element (when it finishes).
     */
    bool finalizeThreadElement(Threaded_Client_Control * x);

private:
    bool initialized, markOnEmptyList;
    Stream_Socket * acceptorSocket;
    pthread_t acceptorThread;
    std::list<Threaded_Client_Control *> threadList;

    bool (*CallbackFunction)(void *,Stream_Socket *);
    void * obj;
    Mutex_Instance sem, endingPhase;
};

#endif // THREADEDACCEPTEDCONTROL_H
