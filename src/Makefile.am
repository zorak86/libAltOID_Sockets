lib_LTLIBRARIES = libalt_sockets.la

pkginclude_HEADERS = win32compat/win32netcompat.h socket_tls_tcp.h socket_udp.h SSL/micro_ssl.h socket_unix.h socket.h datagram_socket.h Pipes/stream_pipe.h Pipes/stream_pipe_thread_base.h ThreadingControl/threaded_stream_acceptor.h ThreadingControl/threaded_client_control.h socket_tcp.h SOSProtocol404/SOSProtocol404Socket.h stream_socket.h

libalt_sockets_la_SOURCES = win32compat/win32netcompat.h win32compat/win32netcompat.cpp socket_tls_tcp.h socket_udp.h SSL/micro_ssl.cpp SSL/micro_ssl.h stream_socket.cpp socket_unix.h socket.h datagram_socket.h socket_tcp.cpp datagram_socket.cpp socket.cpp Pipes/stream_pipe.h Pipes/stream_pipe.cpp Pipes/stream_pipe_thread_base.h Pipes/stream_pipe_thread_base.cpp socket_tls_tcp.cpp ThreadingControl/threaded_stream_acceptor.cpp ThreadingControl/threaded_client_control.cpp ThreadingControl/threaded_stream_acceptor.h ThreadingControl/threaded_client_control.h socket_unix.cpp socket_tcp.h SOSProtocol404/SOSProtocol404Socket.cpp SOSProtocol404/SOSProtocol404Socket.h socket_udp.cpp stream_socket.h
libalt_sockets_la_CPPFLAGS = -I$(prefix)/include -I$(srcdir)/SSL $(OPENSSL_INCLUDES) $(PTHREAD_CFLAGS)
libalt_sockets_la_LDFLAGS = -L$(prefix)/lib -lalt_mutex $(AM_LDFLAGS) $(OPENSSL_LDFLAGS) $(OPENSSL_LIBS) $(PTHREAD_LIBS) -no-undefined -export-dynamic -version-info @version_info@
