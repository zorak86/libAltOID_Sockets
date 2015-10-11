#-------------------------------------------------
#
# Project created by QtCreator 2015-10-08T19:20:05
#
#-------------------------------------------------
QT       -= core gui
CONFIG += c++11

SOURCES += \
    src/SOSProtocol404/SOSProtocol404Socket.cpp \
    src/datagram_socket.cpp \
    src/socket_tcp.cpp \
    src/socket_tls_tcp.cpp \
    src/socket_udp.cpp \
    src/socket_unix.cpp \
    src/socket.cpp \
    src/stream_socket.cpp \
    src/ThreadingControl/threaded_client_control.cpp \
    src/ThreadingControl/threaded_stream_acceptor.cpp

HEADERS += \
    src/SOSProtocol404/SOSProtocol404Socket.h \
    src/datagram_socket.h \
    src/socket_tcp.h \
    src/socket_tls_tcp.h \
    src/socket_udp.h \
    src/socket_unix.h \
    src/socket.h \
    src/stream_socket.h \
    src/ThreadingControl/threaded_client_control.h \
    src/ThreadingControl/threaded_stream_acceptor.h

isEmpty(PREFIX) {
    PREFIX = /usr/local
}

# includes dir
QMAKE_INCDIR += $$PREFIX/include
QMAKE_INCDIR += src
INCLUDEPATH += $$PREFIX/include
INCLUDEPATH += src
# C++ standard.
QMAKE_CXX += -Wno-write-strings -Wno-unused-parameter -Wno-unused-function -O3 -std=c++11 -Wunused -Wno-unused-result
# LIB DEFS:
TARGET = AltOID_Sockets
TEMPLATE = lib
VERSION      = 2.0.1
# INSTALLATION:
target.path = $$PREFIX/lib
header_files.files = $$HEADERS
header_files.path = $$PREFIX/include/alt_sockets
INSTALLS += target
INSTALLS += header_files
# PKGCONFIG
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_LIBDIR = $$PREFIX/lib/
QMAKE_PKGCONFIG_INCDIR = $$PREFIX/include/alt_sockets
QMAKE_PKGCONFIG_CFLAGS = -I$$PREFIX/include/
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

DISTFILES += \
    LICENSE \
    AUTHORS \
    ChangeLog \
    INSTALL \
    README.md

