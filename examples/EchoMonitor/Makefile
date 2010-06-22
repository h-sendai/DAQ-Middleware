ifndef ROOTSYS
$(error This component uses ROOT to draw histogram.  You have to define ROOTSYS environment variable (for example "export ROOTSYS=/usr/local/root" in shell command line))
endif

COMP_NAME = EchoMonitor

all: $(COMP_NAME)Comp

CPPFLAGS += -I/usr/include/daqmw
LDLIBS   += -L/usr/lib/daqmw -lJsonSpirit

CPPFLAGS += -I$(shell ${ROOTSYS}/bin/root-config --incdir)
# root-config --glibs prepend -L
LDLIBS   += $(shell ${ROOTSYS}/bin/root-config --glibs)

LDLIBS   += -lboost_regex

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp
SRCS += ConditionEchoMonitor.cpp

# sample install target
#
# MODE = 0755
# BINDIR = /tmp/mybinary
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

include /usr/share/daqmw/mk/comp.mk
