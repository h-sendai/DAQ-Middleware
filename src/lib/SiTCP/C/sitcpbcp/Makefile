LIBNAME = sitcpbcp
TARGET  = lib$(LIBNAME).a lib$(LIBNAME).so
API_VERSION = 1
PATCHLEVEL  = 0

all: $(TARGET)

INC_DIRS += ../sock

SRCS += sitcpbcp.c
API_INCLUDE_FILES += sitcpbcp.h

sitcpbcp.o:  sitcpbcp.h
sitcpbcp.so: sitcpbcp.h

include ../../../lib.mk
