CC = g++
PROG = sample
CXXFLAGS = -g -O2 -Wall
LDLIBS += -L/usr/lib64/daqmw -lSitcpRbcp -lSock

OBJS += sample.o

all: $(PROG)

$(PROG): $(OBJS)

clean:
	rm -f *.o $(PROG)
