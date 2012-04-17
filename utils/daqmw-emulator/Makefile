DESTDIR     = 
prefix      = /usr

EXEC_MODE     = 0755
SRC_FILE_MODE = 0644

BINDIR    = $(DESTDIR)$(prefix)/bin
SRCDIR    = $(DESTDIR)$(prefix)/share/daqmw/daqmw-emulator

PROG   = daqmw-emulator
CFLAGS = -g -O2 -Wall
LDLIBS = -lm

all: $(PROG)

OBJS += $(PROG).o
OBJS += get_num.o
OBJS += my_signal.o
OBJS += send_data.o
OBJS += prepare_send_data.o
OBJS += gaussian_rand.o

$(PROG): $(OBJS)

FILES += daqmw-emulator.c
FILES += daqmw-emulator.h
FILES += gaussian_rand.c
FILES += get_num.c
FILES += get_num.h
FILES += my_signal.c
FILES += my_signal.h
FILES += prepare_send_data.c
FILES += send_data.c

install:
	install -d $(BINDIR)
	install -m $(EXEC_MODE) $(PROG) $(BINDIR)
	install -d $(SRCDIR)
	install -m $(SRC_FILE_MODE) $(FILES) $(SRCDIR)
	install -m $(SRC_FILE_MODE) Makefile.in $(SRCDIR)/Makefile

uninstall:
	rm -f $(BINDIR)/$(PROG)
	rm -f $(SRCDIR)/*

clean:
	rm -f *.o $(PROG)
