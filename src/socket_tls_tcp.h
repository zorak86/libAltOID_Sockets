#ifndef SOCKET_SSL_TCP_H
#define SOCKET_SSL_TCP_H

#include "socket_tcp.h"
#include <unistd.h>
#include <memory>
#include <string>

#include <openssl/err.h>
#include <openssl/ssl.h>

enum SSL_MODE {
    SSL_MODE_TLS_12
};

struct Micro_SSL {
	Micro_SSL()
	{
	    sslHandle = NULL;
        sslContext = NULL;
	}
    ~Micro_SSL()
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

    void InitHandle()
    {
        sslHandle = SSL_new (sslContext);;
    }

    bool SetFD(int sock)
    {
        if (!sslHandle) return false;
        return SSL_set_fd (sslHandle, sock);
    }

    bool Connect()
    {
        if (!sslHandle) return false;
        return (SSL_connect (sslHandle) == 1);
    }

    bool Accept()
    {
        if (!sslHandle) return false;
        if (SSL_accept(sslHandle) != 1)
        {
#ifndef _WIN32
            ERR_print_errors_fp (stderr);
#else
            //TODO: translate this into win32 compatible source.
#endif
            return false;
        }
        else
        {
            return true;
        }
    }


    SSL *sslHandle;
    SSL_CTX *sslContext;
};


/**
 * TCP Socket Class
 */
class Socket_TLS_TCP : public Socket_TCP {
public:
	/**
	 * Class constructor.
	 */
	Socket_TLS_TCP();
	/**
	 * Class destructor.
	 */
	virtual ~Socket_TLS_TCP();
    /**
     * TLS server function for protocol initialization , it runs in blocking mode and should be called apart to avoid tcp accept while block
     * @return returns true if was properly initialized.
     */
    bool PostAcceptSubInitialization();

    /**
     * Accept a new TCP connection on a listening socket.
     * @return returns a socket with the new established tcp connection.
     */
    Stream_Socket *acceptConnection();

    /**
     * Read a data block from the TLS socket
     * Receive the data block in only one command (without chunks).
     * note that this haves some limitations. some systems can only receive 4k at time.
     * You may want to manage the chunks by yourself.
     * @param data data block.
     * @param datalen data length in bytes
     * @return return the number of bytes read by the socket, zero for end of file and -1 for error.
     */
    virtual int partialRead(void * data, uint32_t datalen);
    /**
     * Write a data block to the TLS socket
     * note that this haves some limitations. some systems can only send 4k at time.
     * You may want to manage the chunks by yourself.
     * @param data data block.
     * @param datalen data length in bytes
     * @return return the number of bytes read by the socket, zero for end of file and -1 for error.
     */
    virtual int partialWrite(void * data, uint32_t datalen);

    /////////////////////////
    // SSL functions:

    /**
     * Call this when program starts:
     */
    static void SSLPrepare();
    /**
     * SSL protocol set.
     */
    void setTLSContextmode(const SSL_MODE &value);
    /**
     * Set SSL certificate chain file.
     * @return true if succeed
     */
    bool setSSLCertificateChainFile(const char * _ca_file);
    /**
     * Set SSL Local certificate file.
     * @return true if succeed
     */
    bool setSSLLocalCertificateFile(const char * _crt_file);
    /**
     * Set SSL Local certificate file.
     * @return true if succeed
     */
    bool setSSLPrivateKeyFile(const char * _key_file);

    void setServer(bool value);

protected:
    /**
     * function for TLS client protocol initialization after the connection starts (client-mode)...
     * @return returns true if was properly initialized.
     */
    bool PostConnectSubInitialization();

private:
    bool InitSSLContext();

    /**
     * @brief ssl
     * this parameter is a shared pointer of ssl connection (copy will reference it.)
     */
    std::shared_ptr<Micro_SSL> ssl;

    std::string crt_file,key_file,ca_file;
    SSL_MODE sslMode;
    bool server;
};

#endif // SOCKET_TCP_H
