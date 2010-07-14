COMP_NAME = SampleMonitor

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

CPPFLAGS += -I$(shell ${ROOTSYS}/bin/root-config --incdir)
LDLIBS   += $(shell ${ROOTSYS}/bin/root-config --glibs)

# sample install target
#
MODE = 0755
BINDIR = /home/daq/bin
#
install: $(COMP_NAME)Comp
	mkdir -p $(BINDIR)
	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

ifdef DAQMWSRCROOT
include ${DAQMWSRCROOT}/src/mk/comp.mk
else
include /usr/share/daqmw/mk/comp.mk
endif
