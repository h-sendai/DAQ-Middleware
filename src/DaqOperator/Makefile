COMP_NAME = DaqOperator

all: $(COMP_NAME)Comp

CPPFLAGS += -I/usr/include/daqmw

LDLIBS += -L/usr/lib/daqmw -lSock
LDLIBS += -lxerces-c

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp
SRCS += ConfFileParser.cpp
SRCS += CreateDom.cpp
SRCS += ServerSocket.cpp

include /usr/share/daqmw/mk/comp.mk
