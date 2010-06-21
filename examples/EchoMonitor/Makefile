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

include /usr/share/daqmw/mk/comp.mk
