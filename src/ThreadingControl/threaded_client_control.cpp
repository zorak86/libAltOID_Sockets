#include "threaded_client_control.h"
#include "threaded_stream_acceptor.h"

void *ClientThread(void *d)
{
    Threaded_Client_Control * threadClient = (Threaded_Client_Control *)d;
    Threaded_Stream_Acceptor * threadedAcceptedControl = (Threaded_Stream_Acceptor *)threadClient->GetParent();
    threadClient->CallCallback();
    threadedAcceptedControl->finalizeThreadElement(threadClient);
    pthread_exit(nullptr);
}

Threaded_Client_Control::Threaded_Client_Control()
{
	CallbackFunction = nullptr;
	clientThread = 0;
	parent = nullptr;
	obj = nullptr;
    clientSocket = nullptr;
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
    int r = pthread_create(&clientThread, nullptr, ClientThread, this);
    pthread_setname_np(clientThread, "ACCPTD_CLI");
    if (!r) pthread_detach(clientThread);
}

void Threaded_Client_Control::StopSocket()
{
    if (clientSocket) clientSocket->shutdownSocket();
}
/*
void Threaded_Client_Control::Join()
{
    pthread_join(clientThread,nullptr);
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
        if (!this->CallbackFunction(obj, clientSocket))
        {
            clientSocket = nullptr;
        }
    }
}

void Threaded_Client_Control::setSocket(Stream_Socket *_clientSocket)
{
    clientSocket = _clientSocket;
}
