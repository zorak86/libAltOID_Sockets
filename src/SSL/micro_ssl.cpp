#include "micro_ssl.h"
#include <unistd.h>

#ifdef _WIN32
#include <openssl/safestack.h>
#endif

Micro_SSL::Micro_SSL()
{
    sslHandle = nullptr;
    sslContext = nullptr;
}

Micro_SSL::~Micro_SSL()
{
    if (sslHandle)
    {
        //        SSL_shutdown (sslHandle);
        SSL_free (sslHandle);
        sslHandle = nullptr;
    }
    if (sslContext)
    {
        SSL_CTX_free(sslContext);
        sslContext = nullptr;
    }
}

bool Micro_SSL::InitContext(bool serverMode)
{
    if (serverMode)
    {
        sslContext = SSL_CTX_new (TLSv1_2_server_method());
        if (!sslContext) return false;
    }
    else
    {
        sslContext = SSL_CTX_new (TLSv1_2_client_method());
        if (!sslContext) return false;
    }
    return true;
}

bool Micro_SSL::isInitialized()
{
    bool r = sslHandle!=nullptr;
    return r;
}

bool Micro_SSL::setCA(const std::string &file)
{
    if (SSL_CTX_load_verify_locations(sslContext, file.c_str(),nullptr) == 1)
    {
        list = SSL_load_client_CA_file( file.c_str() );
        if( list != nullptr )
        {
            SSL_CTX_set_client_CA_list( sslContext, list );
            // It takes ownership. (list now belongs to sslContext)
        }
        return true;
    }
    return false;
}

bool Micro_SSL::setCRT(const std::string &file)
{
    return SSL_CTX_use_certificate_file(sslContext, file.c_str(), SSL_FILETYPE_PEM) == 1;
}

bool Micro_SSL::setKEY(const std::string &file)
{
    return SSL_CTX_use_PrivateKey_file(sslContext, file.c_str(), SSL_FILETYPE_PEM) == 1;
}

void Micro_SSL::InitHandle(bool validatePeer)
{
    sslHandle = SSL_new(sslContext);
    
    if (validatePeer)
    {
        SSL_set_verify(sslHandle, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
    }
}

bool Micro_SSL::SetFD(int sock)
{
    if (!sslHandle) return false;
    return SSL_set_fd (sslHandle, sock) == 1;
}

bool Micro_SSL::Connect()
{
    if (!sslHandle) return false;

    switch ( SSL_get_error(sslHandle, SSL_connect (sslHandle)) )
    {
    case SSL_ERROR_NONE:
        return true;
    default:
        parseErrors();
        return false;
    }
}

bool Micro_SSL::Accept()
{
    if (!sslHandle) return false;
    if (SSL_accept(sslHandle) != 1)
    {
        parseErrors();
        return false;
    }
    else
    {
        return true;
    }
}

size_t Micro_SSL::PartialRead(void *buffer, size_t len)
{
    int readBytes = SSL_read(sslHandle, buffer, len);
    if (readBytes > 0)
    {
        return readBytes;
    }
    else
    {
        switch(SSL_get_error(sslHandle, readBytes))
        {
        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_READ:
            parseErrors();
            return -1;
        case SSL_ERROR_ZERO_RETURN:
            // Socket closed.
            parseErrors();
            return ((size_t)-1);
        default:
            parseErrors();
            return ((size_t)-1);
        }
    }
}

size_t Micro_SSL::PartialWrite(const void *buffer, size_t len)
{
    int sentBytes = SSL_write(sslHandle, buffer, len);
    if (sentBytes > 0)
    {
        return sentBytes;
    }
    else
    {
        switch(SSL_get_error(sslHandle, sentBytes))
        {
        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_READ:
            // Must wait a little bit until the socket buffer is free
            usleep(100000);
            return 0;
        case SSL_ERROR_ZERO_RETURN:
            // Socket closed...
            parseErrors();
            return ((size_t)-1);
        default:
            // Another SSL Error.
            parseErrors();
            return ((size_t)-1);
        }
    }
}

std::list<std::string> Micro_SSL::getErrorsAndClear()
{
    std::list<std::string> r = errors;
    errors.clear();
    return r;
}

std::string Micro_SSL::getPeerCommonName()
{
    char certCNText[512]="";
    X509 * cert = SSL_get_peer_certificate(sslHandle);    
    if(cert)
    {
        X509_NAME * certName = X509_get_subject_name(cert);
        if (certName)
        {
            X509_NAME_get_text_by_NID(certName,NID_commonName,certCNText,511);
        }
        X509_free(cert);
    }
    return std::string(certCNText);
}

std::string Micro_SSL::getCipherName()
{
    return SSL_get_cipher_name(sslHandle);
}

cipherBits Micro_SSL::getCipherBits()
{
    cipherBits cb;
    cb.aSymBits = SSL_get_cipher_bits(sslHandle, &cb.symBits);
    return cb;
}

std::string Micro_SSL::getCipherVersion()
{
    return SSL_get_cipher_version(sslHandle);
}

bool Micro_SSL::validateConnection()
{
    X509 *cert;
    bool bValid  = false;
    cert = SSL_get_peer_certificate(sslHandle);
    if ( cert )
    {
        long res = SSL_get_verify_result(sslHandle);
        if (res == X509_V_OK)
        {
            bValid = true;
        }
        else
        {
            errors.push_back("TLS/SSL Certificate Error: " + std::to_string(res));
        }
        X509_free(cert);
    }

    return bValid;
}

void Micro_SSL::parseErrors()
{
    char buf[512];
    unsigned long int err;
    while ((err = ERR_get_error()) != 0)
    {
        ERR_error_string_n(err, buf, sizeof(buf));
        errors.push_back(buf);
    }
}
