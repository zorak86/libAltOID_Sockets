#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = libAltOIDSockets.la

#######################################
# Build information for each library

# Sources for libAltOIDSockets
libAltOIDSockets_la_SOURCES = socket_udp.cpp ThreadingControl/threaded_stream_acceptor.cpp ThreadingControl/threaded_client_control.cpp socket_unix.cpp SOSProtocol404/SOSProtocol404Socket.cpp socket.cpp datagram_socket.cpp socket_tcp.cpp stream_socket.cpp

# Linker options libTestProgram
libAltOIDSockets_la_LDFLAGS = 

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
libAltOIDSockets_la_CPPFLAGS = -I$(top_srcdir)/include -Wno-write-strings

if DEBUG
AM_CFLAGS = -g3 -O0
AM_CXXFLAGS = -g3 -O0
else
AM_CFLAGS = -O2
AM_CXXFLAGS = -O2
endif
