#include "threaded_stream_acceptor.h"
#include <algorithm>

void *ControlThread(void *d)
{
    Threaded_Stream_Acceptor * threadMasterControl = (Threaded_Stream_Acceptor *)d;
    // Accept until it fails:
    while (threadMasterControl->acceptClient()) {}
    pthread_exit(NULL);
    return NULL;
}

Threaded_Stream_Acceptor::Threaded_Stream_Acceptor()
{
	CallbackFunction = NULL;
	acceptorThread = 0;
	acceptorSocket = NULL;
	obj = NULL;
    markOnEmptyList = false;
    initialized = false;
}

bool Threaded_Stream_Acceptor::acceptClient()
{    
    // Accept each client (create a new thread to manage that client).
    Stream_Socket * clientSocket = acceptorSocket->acceptConnection();

    if (!clientSocket) return true; // BAD!

    // Before creating the new thread, verify...
    // Try blocking endingPhase... if we can't allocate we are on ending phase (drop the connection...)
    if (endingPhase.TryLock()==0)
    {
        Threaded_Client_Control * clientThread = new Threaded_Client_Control;
        clientThread->setSocket(clientSocket);
        clientThread->SetCallbackFunction(CallbackFunction, obj);
        clientThread->SetParent(this);
        if (1==1)
        {
            Locker_Mutex_WR lock(&rwl);
            threadList.push_back(clientThread);
            clientThread->Start();
        }
        // Re-enable for next iteration :)
        endingPhase.UnLock();
        return true;
    }
    else
    {
        // this means that we are not accepting anymore.
        // We should stop here. (clientSocket will be closed because there is no other instance of it)
        clientSocket->shutdownSocket();
        delete clientSocket; // clientSocket not proccesed.
        return false;
    }
}

bool Threaded_Stream_Acceptor::finalizeThreadElement(Threaded_Client_Control *x)
{
	Locker_Mutex_WR lock(&rwl);
    if (std::find(threadList.begin(), threadList.end(), x) == threadList.end()) return false;
    x->Join();
    threadList.remove(x);
    delete x;

    // Don't unlock until it's empty and it's requested.
    if (markOnEmptyList && threadList.empty()) sem.UnLock(); // Pass!
    return true;
}

void Threaded_Stream_Acceptor::setCallbackFunction(bool (*_CallbackFunction)(void *, Stream_Socket *), void *obj)
{
    CallbackFunction = _CallbackFunction;
    this->obj = obj;
}

void Threaded_Stream_Acceptor::setAcceptorSocket(Stream_Socket * acceptorSocket)
{
    this->acceptorSocket = acceptorSocket;
}

Threaded_Stream_Acceptor::~Threaded_Stream_Acceptor()
{
    // Now we are on ending phase, new threads are not allowed.
    // Here we ensure that no threads are comming to be injected.
    endingPhase.Lock();

    // Shutdown the thread.
    acceptorSocket->shutdownSocket();

    // Accept the listen-injection thread. (no new threads will be added from here)
    pthread_join(acceptorThread,NULL);

    // Now we can safetly free the acceptor socket resource.
    acceptorSocket->closeSocket();

    if (1==1)
    {
        // Don't accept finalization messages from here. (a queue of finalized threads should be trying to announce their intentions to finalize the socket)
    	Locker_Mutex_WR lock(&rwl);

        // Finalization threads should mark if they are the last thread being there.
        markOnEmptyList = true;

        // And lock is enforced if there is any thread on the queue... if there is no thread... no queue should be comming so no unlock needed
        if (threadList.size()) sem.Lock();

        // Send stopsocket on every child thread (if there are).
        for (Threaded_Client_Control * thread : threadList)
        {
            thread->StopSocket();
        }
    }
    // Then, each thread should fall independently... We should wait until it happens.

    // WAIT until it falls to zero. and because no more threads are accepted and added, zero should be sufficient to say: its over.
    sem.Lock();

    // Now everything is finished :) proceeding to delete the acceptor.
    delete acceptorSocket;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NEVER DELETE THIS CLASS AFTER THE START, JUST STOP AND WILL BE DELETED
bool Threaded_Stream_Acceptor::start()
{
    if (!pthread_create(&acceptorThread, NULL, ControlThread, this))
    {
        initialized = true;
#ifndef _WIN32
        pthread_setname_np(acceptorThread, "STR_ACCPTOR");
#endif
        return true;
    }
    else
    {
        return false;
    }
}

void Threaded_Stream_Acceptor::stop()
{
    acceptorSocket->shutdownSocket();
}
