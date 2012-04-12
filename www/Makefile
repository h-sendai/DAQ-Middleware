include Distributions.mk

DESTDIR =

ifeq ($(strip $(USE_MOD_WSGI)),1)
SUBDIRS = mod_wsgi
endif

ifeq ($(strip $(USE_MOD_PYTHON)),1)
SUBDIRS = mod_python
endif

.PHONY: subdirs $(SUBDIRS)

all: $(SUBDIRS)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

clean:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

install:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

uninstall:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
