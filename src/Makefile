DESTDIR =
prefix  = /usr

SUBDIRS += lib
SUBDIRS += mk
SUBDIRS += DaqComponent
SUBDIRS += DaqOperator

SUBDIRS_CLEAN += lib
SUBDIRS_CLEAN += DaqComponent
SUBDIRS_CLEAN += DaqOperator
SUBDIRS_CLEAN += mk

.PHONY: $(SUBDIRS)

all: $(SUBDIRS)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

clean:
	@set -e; for dir in $(SUBDIRS_CLEAN); do $(MAKE) -C $${dir} $@; done
	
install:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

uninstall:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
