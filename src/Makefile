prefix = /usr

SUBDIRS += lib
SUBDIRS += DaqComponent
SUBDIRS += DaqOperator
SUBDIRS += DaqService
SUBDIRS += mk

.PHONY: $(SUBDIRS)

all: $(SUBDIRS)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

clean:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
	
install:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

uninstall:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
