# libAltOID_Socket

Alternative OpenSource Socket Library - by zorak x86   
License: LGPL v3  
Version: 2.0.1 (untested library)
Requires: libAltOID_Mutex  

## Usage

You should compile with the following flags (or use the .pc)
-L$$PREFIX/lib -lAltOID_Sockets -lssl -lcrypto

## Functionality

This library provides C++ abstraction on:  

- TCP Sockets
- TLS Sockets
- UDP Sockets
- Unix Sockets

And some features like:  

- Very basic interface for sending/manipulating data containers.
- A class for automating listening client threads
