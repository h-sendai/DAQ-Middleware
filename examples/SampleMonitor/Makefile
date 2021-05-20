# First we check ROOTSYS environment variable.

ifndef ROOTSYS
$(error This program requires ROOTSYS environment variable\
but does not defined.  Please define ROOTSYS as follows at\
shell prompt: "export ROOTSYS=/usr/local/root".  If you don't install\
ROOT in /usr/local/root, please substiture your ROOT root directory)
endif

# Second we check root-config command to lookup the ROOT is installed or not.

ROOT_INC = $(shell ${ROOTSYS}/bin/root-config --incdir)
ifeq ($(strip $(ROOT_INC)),)
$(error Cannot execute root-config command at $$ROOTSYS/bin/root-config.\
Please install ROOT program.\
Consult at http://root.cern.ch to install ROOT)
endif

# Then (probably) OK.

COMP_NAME = SampleMonitor

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

CXXFLAGS += $(shell ${ROOTSYS}/bin/root-config --cflags)
LDLIBS   += $(shell ${ROOTSYS}/bin/root-config --glibs)

# sample install target
#
# MODE = 0755
# BINDIR = /home/daq/bin
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

include /usr/share/daqmw/mk/comp.mk
