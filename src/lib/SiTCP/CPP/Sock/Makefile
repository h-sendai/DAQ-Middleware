LIBNAME = Sock
TARGET  = lib$(LIBNAME).a lib$(LIBNAME).so

all: $(TARGET)

CPPSRCS   += Sock.cpp
API_INCLUDE_FILES += Sock.h

Sock.o:  Sock.h
Sock.so: Sock.h

include ../../../lib.mk
