COMP_NAME = DaqOperator

all: $(COMP_NAME)Comp

CPPFLAGS += -I/usr/include/daqmw

LDLIBS += -L/usr/lib/daqmw -lSock
LDLIBS += -lxerces-c

OBJS += ConfFileParser.o
OBJS += CreateDom.o
OBJS += ServerSocket.o

include /usr/share/daqmw/mk/comp.mk
