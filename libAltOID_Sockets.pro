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
    src/socket_udp.cpp \
    src/socket.cpp \
    src/stream_socket.cpp \
    src/ThreadingControl/threaded_client_control.cpp \
    src/ThreadingControl/threaded_stream_acceptor.cpp \
    src/Pipes/stream_pipe.cpp \
    src/Pipes/stream_pipe_thread_base.cpp

HEADERS += \
    src/SOSProtocol404/SOSProtocol404Socket.h \
    src/datagram_socket.h \
    src/socket_tcp.h \
    src/socket_udp.h \
    src/socket.h \
    src/stream_socket.h \
    src/ThreadingControl/threaded_client_control.h \
    src/ThreadingControl/threaded_stream_acceptor.h \
    src/Pipes/stream_pipe.h \
    src/Pipes/stream_pipe_thread_base.h

isEmpty(PREFIX) {
    PREFIX = /usr/local
}

!disable_ssl {
    SOURCES += src/socket_tls_tcp.cpp src/SSL/micro_ssl.cpp
    HEADERS += src/socket_tls_tcp.h src/SSL/micro_ssl.h
}

!win32 {
    SOURCES += src/socket_unix.cpp
    HEADERS += src/socket_unix.h
}
win32 {
    SOURCES += src/win32compat/win32netcompat.cpp
    HEADERS += src/win32compat/win32netcompat.h
}

# includes dir
QMAKE_INCDIR += $$PREFIX/include
QMAKE_INCDIR += src
!disable_ssl {
    QMAKE_INCDIR += src/SSL
}
INCLUDEPATH += $$PREFIX/include
INCLUDEPATH += src
!disable_ssl {
    INCLUDEPATH += src/SSL
}
# C++ standard.
QMAKE_CXX += -Wno-write-strings -Wno-unused-parameter -Wno-unused-function -O3 -std=c++11 -Wunused -Wno-unused-result

MINGWVERSION = mingw530_32

# Aditional includes...
win32:INCLUDEPATH += C:/Qt/Tools/$$MINGWVERSION/opt/include
win32:QMAKE_INCDIR += C:/Qt/Tools/$$MINGWVERSION/opt/include
win32:LIBS += -LC:/Qt/Tools/$$MINGWVERSION/opt/lib -L$$PREFIX\lib

# LIB DEFS:
!disable_ssl {
win32:LIBS += -lssl -lcrypto
win32:LIBS += -lws2_32
}

CONFIG(debug, debug|release) {
win32:LIBS += -lAltOID_Mutexd1
} else {
win32:LIBS += -lAltOID_Mutex1
}

TARGET = AltOID_Sockets
TEMPLATE = lib
VERSION      = 2.3.4
# INSTALLATION:
target.path = $$PREFIX/lib
header_files.files = $$HEADERS
header_files.path = $$PREFIX/include/alt_sockets
INSTALLS += target
INSTALLS += header_files

DISTFILES += \
    LICENSE \
    AUTHORS \
    ChangeLog \
    INSTALL \
    README.md

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,_debug)
    else: TARGET = $$join(TARGET,,,d)
}

# PKGCONFIG
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_LIBDIR = $$PREFIX/lib/
QMAKE_PKGCONFIG_INCDIR = $$PREFIX/include/alt_sockets
QMAKE_PKGCONFIG_CFLAGS = -I$$PREFIX/include/
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
