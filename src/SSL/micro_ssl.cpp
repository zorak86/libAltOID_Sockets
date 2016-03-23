#include "micro_ssl.h"
#include <unistd.h>

Micro_SSL::Micro_SSL()
{
    sslHandle = nullptr;
    sslContext = nullptr;
}

Micro_SSL::~Micro_SSL()
{
    if (sslHandle)
    {
        SSL_shutdown (sslHandle);
        SSL_free (sslHandle);
    }
    if (sslContext)
    {
        SSL_CTX_free(sslContext);
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
    return sslHandle!=nullptr;
}

bool Micro_SSL::setCA(const std::__cxx11::string &file)
{
    return SSL_CTX_use_certificate_chain_file(sslContext, file.c_str()) == 1;
}

bool Micro_SSL::setCRT(const std::__cxx11::string &file)
{
    return SSL_CTX_use_certificate_file(sslContext, file.c_str(), SSL_FILETYPE_PEM) == 1;
}

bool Micro_SSL::setKEY(const std::__cxx11::string &file)
{
    return SSL_CTX_use_PrivateKey_file(sslContext, file.c_str(), SSL_FILETYPE_PEM) == 1;
}

void Micro_SSL::InitHandle()
{
    sslHandle = SSL_new(sslContext);
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
        break;
    default:
        parseErrors();
        return false;
        break;
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
    ssize_t readBytes = SSL_read(sslHandle, buffer, len);
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
            break;
        case SSL_ERROR_ZERO_RETURN:
            // Socket closed.
            parseErrors();
            return -1;
            break;
        default:
            parseErrors();
            return -1;
            break;
        }
    }
}

size_t Micro_SSL::PartialWrite(void *buffer, size_t len)
{
    ssize_t sentBytes = SSL_write(sslHandle, buffer, len);
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
            break;
        case SSL_ERROR_ZERO_RETURN:
            // Socket closed...
            parseErrors();
            return -1;
            break;
        default:
            // Another SSL Error.
            parseErrors();
            return -1;
            break;
        }
    }
}

std::list<std::__cxx11::string> Micro_SSL::getErrorsAndClear()
{
    std::list<std::string> r = errors;
    errors.clear();
    return r;
}

std::__cxx11::string Micro_SSL::getCipherName()
{
    return SSL_get_cipher_name(sslHandle);
}

cipherBits Micro_SSL::getCipherBits()
{
    cipherBits cb;
    cb.aSymBits = SSL_get_cipher_bits(sslHandle, &cb.symBits);
    return cb;
}

std::__cxx11::string Micro_SSL::getCipherVersion()
{
    return SSL_get_cipher_version(sslHandle);
}

void Micro_SSL::parseErrors()
{
    char buf[512];
    u_long err;
    while ((err = ERR_get_error()) != 0)
    {
        ERR_error_string_n(err, buf, sizeof(buf));
        errors.push_back(buf);
    }
}
