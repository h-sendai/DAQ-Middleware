LIBNAME = SitcpRbcp
TARGET  = lib$(LIBNAME).a lib$(LIBNAME).so
API_VERSION = 1
PATCHLEVEL  = 0

all: $(TARGET)

CPPSRCS   += SitcpRbcp.cpp
API_INCLUDE_FILES += SitcpRbcp.h

SitcpRbcp.o:  SitcpRbcp.h
SitcpRbcp.so: SitcpRbcp.h

CPPFLAGS += -I../Sock -I.

include ../../../lib.mk
