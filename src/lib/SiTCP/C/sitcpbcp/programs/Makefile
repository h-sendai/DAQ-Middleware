PROGS += readReg
PROGS += writeReg

all: $(PROGS)

CPPFLAGS = -I.. -I../../sock
LDFLAGS  = -L.. -L../../sock
LDLIBS   = -lsitcpbcp -lsock

clean:
	rm -f $(PROGS) *.o

install:
