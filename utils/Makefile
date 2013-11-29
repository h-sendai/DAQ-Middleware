DESTDIR =
prefix  = /usr

SUBDIRS += pythonLibSetup
SUBDIRS += daqcom
SUBDIRS += newcomp
SUBDIRS += daqmw-emulator
SUBDIRS += conditionPanel
SUBDIRS += dmbepanel
SUBDIRS += configGUI

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
