# This is makefile for IN SOURCE TREE
# NOT for DAQ-Components Developers, but for DAQ-Middleware Developers
# or DAQ-Middleware Distributers

DESTDIR =
prefix  = /usr
BINDIR  = $(DESTDIR)$(prefix)/libexec/daqmw
SRCDIR  = $(DESTDIR)$(prefix)/share/daqmw/DaqOperator

cwd     = $(shell pwd)
updir   = $(shell dirname $(cwd))
upupdir = $(shell dirname $(updir))
export DAQMWSRCROOT=$(upupdir)

COMP_NAME = DaqOperator

all: $(COMP_NAME)Comp

CPPFLAGS += -I../lib/SiTCP/CPP/Sock
CXXFLAGS += -g
LDLIBS += -L../lib/SiTCP/CPP/Sock -lSock
LDLIBS += -lxerces-c

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp
SRCS += ConfFileParser.cpp
SRCS += CreateDom.cpp

FILES += ComponentInfoContainer.h
FILES += ConfFileParser.cpp
FILES += ConfFileParser.h
FILES += CreateDom.cpp
FILES += CreateDom.h
FILES += DaqOperator.cpp
FILES += DaqOperator.h
FILES += DaqOperatorComp.cpp
FILES += Parameter.h
FILES += ParameterServer.h
FILES += callback.h

install: all
	@mkdir -p $(BINDIR)
	@install -m 755 $(COMP_NAME)Comp $(BINDIR)
	@mkdir -p $(SRCDIR)
	@install -m 644 $(FILES) $(SRCDIR)
	@install -m 644 Makefile.in $(SRCDIR)/Makefile

uninstall:
	@rm -f $(BINDIR)/$(COMP_NAME)Comp
	@rm -f $(SRCDIR)/*

obj:
	rm -rf *.o

include ../mk/comp.mk
