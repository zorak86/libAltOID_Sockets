#include "socket_tls_tcp.h"

#include <openssl/rand.h>
#include <openssl/err.h>

#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////                        TLS-TCP                                 /////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

Socket_TLS_TCP::Socket_TLS_TCP()
{
    server = false;
    // ssl empty, create a new one.
    ssl.reset(new Micro_SSL);   
}

Socket_TLS_TCP::~Socket_TLS_TCP()
{
}

void Socket_TLS_TCP::SSLPrepare()
{
    // Register the error strings for libcrypto & libssl
    SSL_load_error_strings ();
    ERR_load_crypto_strings();
    // Register the available ciphers and digests
    SSL_library_init ();
}

bool Socket_TLS_TCP::PostConnectSubInitialization()
{
    if (ssl->isInitialized()) return false; // already connected (don't connect again)

    server = false;

    if (!InitSSLContext())
    {
        sslErrors = ssl->getErrorsAndClear();
        ssl.reset(new Micro_SSL);
        return false;
    }
    ssl->InitHandle();

    if (!ssl->SetFD(getSocket()))
    {
        sslErrors = ssl->getErrorsAndClear();
        ssl.reset(new Micro_SSL);
        return false;
    }
    else if (!(*ssl).Connect())
    {
        sslErrors = ssl->getErrorsAndClear();
        ssl.reset(new Micro_SSL);
        return false;
    }
    // connected!
    return true;
}

bool Socket_TLS_TCP::PostAcceptSubInitialization()
{
    if (ssl->isInitialized()) return false; // already connected (don't connect again)

    server = true;
    if (!InitSSLContext())
    {
        sslErrors = ssl->getErrorsAndClear();
        ssl.reset(new Micro_SSL);
        return false;
    }

    // ssl empty, create a new one.
    ssl->InitHandle();

    if (!ssl->SetFD(getSocket()))
    {
        sslErrors = ssl->getErrorsAndClear();
        ssl.reset(new Micro_SSL);
        return false;
    }
    else if (!(*ssl).Accept())
    {
        sslErrors = ssl->getErrorsAndClear();
        ssl.reset(new Micro_SSL);
        return false;
    }

    // connected!
    return true;
}

bool Socket_TLS_TCP::InitSSLContext()
{
    // create new SSL Context.
    if (!ssl->InitContext(server)) return false;

    if (!ca_file.empty() && !ssl->setCA(ca_file))
    {
        return false;
    }
    if (!crt_file.empty() && !ssl->setCRT(crt_file))
    {
        return false;
    }
    if (!key_file.empty() && !ssl->setKEY(key_file))
    {
        return false;
    }

    return true;
}

std::list<std::string> Socket_TLS_TCP::getSslErrorsAndClear()
{
    std::list<std::string> sslErrors2 = ssl->getErrorsAndClear();
    for (std::string & i : sslErrors2) sslErrors.push_back(i);
    std::list<std::string> sslErrors3 = sslErrors;
    sslErrors.clear();
    return sslErrors3;
}

void Socket_TLS_TCP::setServer(bool value)
{
    server = value;
}

void Socket_TLS_TCP::setTLSContextmode(const SSL_MODE &value)
{
    sslMode = value;
}

bool Socket_TLS_TCP::setSSLCertificateChainFile(const char *_ca_file)
{
    ca_file = _ca_file;
    return true;
}

bool Socket_TLS_TCP::setSSLLocalCertificateFile(const char *_crt_file)
{
    crt_file = _crt_file;
    return true;
}

bool Socket_TLS_TCP::setSSLPrivateKeyFile(const char *_key_file)
{
    key_file = _key_file;
    return true;
}

string Socket_TLS_TCP::getCipherName()
{
    return ssl->getCipherNames();
}

cipherBits Socket_TLS_TCP::getCipherBits()
{
    return ssl->getCipherBits();
}

string Socket_TLS_TCP::getCipherVersion()
{
    return ssl->getCipherVersion();
}

Stream_Socket * Socket_TLS_TCP::acceptConnection()
{
    Stream_Socket * mainSock = Socket_TCP::acceptConnection();
    if (!mainSock) return NULL;
    Socket_TLS_TCP * tlsSock = new Socket_TLS_TCP; // Convert to this thing...

    server = true;

    // Set current retrieved socket.
    tlsSock->setSocket(mainSock->getSocket());
    char remotePair[64];
    mainSock->getRemotePair(remotePair);
    tlsSock->setRemotePair(remotePair);
    tlsSock->setRemotePort(mainSock->getRemotePort());
    // detach, because tlsSock will rule the socket file descriptor now.
    mainSock->detachSocket();
    // now we should copy the context parameters:
    delete mainSock;

    if (!ca_file.empty()) tlsSock->setSSLCertificateChainFile( ca_file.c_str() );
    if (!crt_file.empty()) tlsSock->setSSLLocalCertificateFile( crt_file.c_str() );
    if (!key_file.empty()) tlsSock->setSSLPrivateKeyFile( key_file.c_str() );

    tlsSock->setTLSContextmode(sslMode);
    tlsSock->setServer(server);

    return tlsSock;
}

int Socket_TLS_TCP::partialRead(void *data, uint32_t datalen)
{
    return ssl->PartialRead(data,datalen);
}

int Socket_TLS_TCP::partialWrite(void *data, uint32_t datalen)
{
    return ssl->PartialWrite(data,datalen);
}
