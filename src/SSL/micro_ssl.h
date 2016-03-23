#ifndef MICRO_SSL_H
#define MICRO_SSL_H

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <string>
#include <list>

class Micro_SSL
{
public:
    Micro_SSL();
    ~Micro_SSL();

    void InitHandle();
    bool InitContext(bool serverMode = false);
    bool isInitialized();

    bool setCA(const std::string & file);
    bool setCRT(const std::string & file);
    bool setKEY(const std::string & file);

    bool SetFD(int sock);
    bool Connect();
    bool Accept();

    size_t PartialRead(void * buffer, size_t len);
    size_t PartialWrite(void * buffer, size_t len);

    std::list<std::string> getErrorsAndClear();

private:
    void parseErrors();

    std::list<std::string> errors;

    SSL *sslHandle;
    SSL_CTX *sslContext;
};

#endif // MICRO_SSL_H
