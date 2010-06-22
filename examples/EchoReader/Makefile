COMP_NAME = EchoReader

all: $(COMP_NAME)Comp

CPPFLAGS += -I/usr/include/daqmw
LDLIBS   += -L/usr/lib/daqmw -lSock

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

include /usr/share/daqmw/mk/comp.mk
