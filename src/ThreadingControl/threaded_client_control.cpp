#include "threaded_client_control.h"
#include "threaded_stream_acceptor.h"

void *ClientThread(void *d)
{
    Threaded_Client_Control * threadClient = (Threaded_Client_Control *)d;
    Threaded_Stream_Acceptor * threadedAcceptedControl = (Threaded_Stream_Acceptor *)threadClient->GetParent();
    threadClient->CallCallback();
    threadedAcceptedControl->finalizeThreadElement(threadClient);
    pthread_exit(NULL);
    return NULL;
}

Threaded_Client_Control::Threaded_Client_Control()
{
	CallbackFunction = NULL;
	clientThread = 0;
	parent = NULL;
	obj = NULL;
    clientSocket = NULL;
}

Threaded_Client_Control::~Threaded_Client_Control()
{
    if (clientSocket)
    {
        clientSocket->closeSocket(); // close the socket when the thread ends...
        delete clientSocket;
    }
}

void Threaded_Client_Control::Start()
{
    int r = pthread_create(&clientThread, NULL, ClientThread, this);
    if (!r) pthread_detach(clientThread);
/*#ifndef _WIN32
    pthread_setname_np(clientThread, "ACCPTD_CLI");
#endif*/
}

void Threaded_Client_Control::StopSocket()
{
    if (clientSocket) clientSocket->shutdownSocket();
}
/*
void Threaded_Client_Control::Join()
{
    pthread_join(clientThread,NULL);
}*/

void Threaded_Client_Control::SetCallbackFunction(bool(*_CallbackFunction)(void *, Stream_Socket *), void *obj)
{
    CallbackFunction = _CallbackFunction;
    this->obj = obj;
}

void Threaded_Client_Control::SetParent(void *parent)
{
    this->parent = parent;
}

void *Threaded_Client_Control::GetParent()
{
    return parent;
}

void Threaded_Client_Control::CallCallback()
{
    // Accept (internal protocol)
    if (clientSocket->PostAcceptSubInitialization())
    {
        // Start
        this->CallbackFunction(obj, clientSocket);
    }
}

void Threaded_Client_Control::setSocket(Stream_Socket *_clientSocket)
{
    clientSocket = _clientSocket;
}
