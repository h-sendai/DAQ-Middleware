# This is makefile for IN SOURCE TREE
# NOT for DAQ-Components Developers, but for DAQ-Middleware Developers
# or DAQ-Middleware Distributers

prefix = /usr
BINDIR = $(prefix)/libexec/daqmw
SRCDIR = $(prefix)/share/daqmw/DaqOperator 

export BASE_PATH=..

COMP_NAME = DaqOperator

all: $(COMP_NAME)Comp

CPPFLAGS += -I../lib/SiTCP/CPP/Sock
LDLIBS += -L../lib/SiTCP/CPP/Sock -lSock
LDLIBS += -lxerces-c

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp
SRCS += ConfFileParser.cpp
SRCS += CreateDom.cpp
SRCS += ServerSocket.cpp

FILES += ComponentInfoContainer.h
FILES += ConfFileParser.cpp
FILES += ConfFileParser.h
FILES += CreateDom.cpp
FILES += CreateDom.h
FILES += DaqOperator.cpp
FILES += DaqOperator.h
FILES += DaqOperatorComp.cpp
FILES += Makefile
FILES += Parameter.h
FILES += ParameterServer.h
FILES += ServerSocket.cpp
FILES += ServerSocket.h
FILES += callback.h

install: all
	@mkdir -p $(BINDIR)
	@install -m 755 $(COMP_NAME)Comp $(BINDIR)
	@mkdir -p $(SRCDIR)
	@install -m 644 $(FILES) $(SRCDIR)

include ../mk/comp.mk
