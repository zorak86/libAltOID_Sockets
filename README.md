# libAltOID_Socket

Alternative OpenSource Socket Library - by zorak x86   
License: LGPL v3  
Version: 3.0.1  
Requires: libalt_mutex libpthread libssl libcrypto  

## Usage

You should compile with the following flags (or use the .pc)  
-lalt_mutex -lssl -lcrypto -lpthread  

## Functionality

This library provides C++ abstraction on:

- TCP Sockets
- TLS Sockets
- UDP Sockets
- Unix Sockets

And some features like:  

- Very basic interface for sending/manipulating data containers.
- A class for automating listening client threads

## Build instructions (using libtool)

```
autoconf -i (or ./autogen.sh)
./configure --prefix=/usr
make -j8
make install
```