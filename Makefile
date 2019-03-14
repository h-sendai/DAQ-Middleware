DESTDIR =
prefix = /usr

VERSION = $(shell cat VERSION)

SUBDIRS += src
SUBDIRS += bin
SUBDIRS += conf
SUBDIRS += docs
SUBDIRS += etc
SUBDIRS += examples
SUBDIRS += utils
SUBDIRS += www

.PHONY: $(SUBDIRS)

all: $(SUBDIRS)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

clean:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
	
install:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
	./install_ld_conf $(DESTDIR)

uninstall:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
	rm -f $(DESTDIR)/etc/ld.so.conf.d/daqmw.conf

dist:
	@git archive HEAD --format=tar.gz --prefix=DAQ-Middleware-$(VERSION)/ -o ~/rpm/SOURCES/DAQ-Middleware-$(VERSION).tar.gz
