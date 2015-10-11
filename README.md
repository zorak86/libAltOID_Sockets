# libAltOID_Socket

Alternative OpenSource Socket Library - by zorak x86   
License: LGPL v3  
Version: 2.0.1
Requires: libAltOID_Mutex libpthread libssl libcrypto

## Usage

You should compile with the following flags (or use the .pc)
-lAltOID_Sockets -lssl -lcrypto -lAltOID_Mutex -lpthread

## Functionality

This library provides C++ abstraction on:  

- TCP Sockets
- TLS Sockets
- UDP Sockets
- Unix Sockets

And some features like:  

- Very basic interface for sending/manipulating data containers.
- A class for automating listening client threads
