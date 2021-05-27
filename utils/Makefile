include ../Distributions.mk

DESTDIR =
prefix  = /usr

SUBDIRS += pythonLibSetup
SUBDIRS += daqcom
SUBDIRS += newcomp
SUBDIRS += daqmw-emulator

ifeq ($(strip $(USE_PYTHON2)),1)
    SUBDIRS += conditionPanel
    SUBDIRS += dmbepanel
    SUBDIRS += configGUI
endif

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
