include ../Distributions.mk

ifeq ($(strip $(USE_PYTHON3)),1)
SUBDIRS = python3
endif

ifeq ($(strip $(USE_PYTHON2)),1)
SUBDIRS = python2
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
