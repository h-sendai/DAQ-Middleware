LIBNAME = sock
API_VERSION = 1
PATCHLEVEL  = 0

TARGET  = lib$(LIBNAME).a lib$(LIBNAME).so

all: $(TARGET)

SRCS   += sock.c
API_INCLUDE_FILES += sock.h

sock.o:  sock.h
sock.so: sock.h

include ../../../lib.mk
