#ifndef SOSPROTOCOL404_H_
#define SOSPROTOCOL404_H_

#include "../stream_socket.h"
#include <string>

/**
 * Simple Open Streaming Protocol 404
 * defines a set of functions to send/recv common C/C++ elements using streamable protocols (TCP/UNIX Sockets).
 * this does not defines protocol version, and the synchronization should be handled by the user.
 */
class SOSProtocol404_Socket : private Stream_Socket {
public:
	/**
	 * Protocol class constructor
	 */
	SOSProtocol404_Socket();
	/**
	 * Protocol class destructor
	 */
	virtual ~SOSProtocol404_Socket();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Basic data structures...
	/**
	 * Read unsigned char
	 * @param readOK pointer to bool variable that will be filled with the result (success or fail).
	 * @return char retrived.
	 * */
    unsigned char readU8(bool *readOK=0x0);
	/**
	 * Write unsigned char
	 * @param c char to write into the socket.
	 * @return true if succeed.
	 * */
    bool writeU8(const unsigned char & c);
	/**
	 * Read unsigned short (16bit)
	 * @param readOK pointer to bool variable that will be filled with the result (success or fail).
	 * @return char retrived.
	 * */
    uint16_t readU16(bool *readOK=0x0);
	/**
	 * Write unsigned short (16bit)
	 * @param c char to write into the socket.
	 * @return true if succeed.
	 * */
    bool writeU16(const uint16_t & c);
	/**
	 * Read unsigned integer (32bit)
	 * @param readOK pointer to bool variable that will be filled with the result (success or fail).
	 * @return char retrived.
	 * */
    uint32_t readU32(bool *readOK=0x0);
	/**
	 * Write unsigned integer (32bit)
	 * @param c char to write into the socket.
	 * @return true if succeed.
	 * */
    bool writeU32(const uint32_t & c);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data blocks send/retrieval:

	//////////////////////////////// 32
	/**
	 * Write data block of maximum of 4g bytes (teotherical).
	 * @param data data block bytes
	 * @param datalen data length to be sent.
	 * @return true if succeed.
	 */
    bool writeBlock32(const void * data, uint32_t datalen);
	/**
	 * Read data block of maximum of 4g bytes (teotherical).
	 * @param data data to allocate the incoming bytes.
	 * @param datalen maximum data length to be allocated.
	 * @param keepDataLen maximum data to be received should match the incoming data, otherwise,return false (and you should close)
	 * @return true if succeed.
	 */
    bool readBlock32(void * data, uint32_t datalen, bool keepDataLen = false);
    /**
	 * Read and allocate a memory space data block of maximum of 4g bytes (teotherical).
	 * NOTE: Allocation occurs with new [], so delete it with delete []
	 * @param datalen in: maximum data length supported, out: data retrieved.
	 * @return memory allocated with the retrieved data or NULL if failed.
     */
    void * readBlock32WAlloc(unsigned int * datalen);
    /**
	 * Read and allocate a memory space data block of maximum of 4g bytes until a delimiter bytes (teotherical).
	 * @param datalen in: maximum data length supported (should be min: 65536), out: data retrieved.
	 * @param delim delimiter.
	 * @param delimBytes delimiter size (max: 65535 bytes).
	 * @return memory allocated with the retrieved data or NULL if failed.
     */
    void * readBlock32WAllocAndDelim(unsigned int * datalen, unsigned char * delim, uint16_t delimBytes);

    //////////////////////////////// 16
	/**
	 * Write data block of maximum of 64k bytes (teotherical).
	 * @param data data block bytes
	 * @param datalen data length to be sent.
	 * @return true if succeed.
	 */
    bool writeBlock16(const void * data, uint16_t datalen);
	/**
	 * Read data block of maximum of 64k bytes (teotherical).
	 * @param data data to allocate the incoming bytes.
	 * @param datalen maximum data length to be allocated.
	 * @param keepDataLen maximum data to be received should match the incoming data, otherwise,return false (and you should close)
	 * @return true if succeed.
	 */
    bool readBlock16(void * data, uint16_t datalen, bool keepDataLen = false);
    /**
	 * Read and allocate a memory space data block of maximum of 64k bytes (teotherical).
	 * NOTE: Allocation occurs with new [], so delete it with delete []
	 * @param datalen in: maximum data length supported, out: data retrieved.
	 * @return memory allocated with the retrieved data or NULL if failed.
     */
    void * readBlock16WAlloc(uint16_t * datalen);

    //////////////////////////////// 8
	/**
	 * Write data block of maximum of 256 bytes (teotherical).
	 * @param data data block bytes
	 * @param datalen data length to be sent.
	 * @return true if succeed.
	 */
    bool writeBlock8(const void * data, uint8_t datalen);
	/**
	 * Read data block of maximum of 256 bytes (teotherical).
	 * @param data data to allocate the incoming bytes.
	 * @param datalen maximum data length to be allocated.
	 * @param keepDataLen maximum data to be received should match the incoming data, otherwise,return false (and you should close)
	 * @return true if succeed.
	 */
    bool readBlock8(void * data, uint8_t datalen, bool keepDataLen = false);
    /**
	 * Read and allocate a memory space data block of maximum of 256 bytes (teotherical).
	 * NOTE: Allocation occurs with new [], so delete it with delete []
	 * @param datalen in: maximum data length supported, out: data retrieved.
	 * @return memory allocated with the retrieved data or NULL if failed.
     */
    void * readBlock8WAlloc(uint8_t * datalen);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Null terminated strings:
    /**
     * Read a std::string of maximum 65kb.
     * @return string with the data contained inside
     */
    std::string readString16(bool *readOK=0x0);
    /**
     * Write a std::string of maximum 65kb.
     * @param str string to be sent.
     * @return true if success
     */
    bool writeString16(const std::string & str);
    /**
     * Read a std::string of maximum 256 bytes.
     * @return string with the data contained inside
     */
    std::string readString8(bool *readOK=0x0);
    /**
     * Write a std::string of maximum 256 bytes.
     * @param str string to be sent.
     * @return true if success
     */
    bool writeString8(const std::string & str);

private:
    int read64KBlockDelim(unsigned char * block, unsigned char* delim, uint16_t delimBytes, unsigned int blockNo);
};

#endif /* SOSPROTOCOL404_H_ */
