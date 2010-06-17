prefix = /usr

VERSION = 1.0.0

SUBDIRS += bin
SUBDIRS += conf
SUBDIRS += docs
SUBDIRS += etc
SUBDIRS += examples
SUBDIRS += src
SUBDIRS += utils

# SUBDIRS += www

.PHONY: $(SUBDIRS)

all: $(SUBDIRS)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

clean:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
	
install:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
#README
#Changes
#INSTALL

uninstall:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

dist:
	@(cd ..; tar zcf DAQ-Middleware-$(VERSION).tar.gz DAQ-Middleware-$(VERSION))
#$(SUBDIRS):
#	@$(MAKE) -C $@
