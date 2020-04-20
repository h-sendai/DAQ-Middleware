include ../../../Distributions.mk

DESTDIR =
DAQ_HTML_DIR = $(DESTDIR)$(WWW_DOCUMENT_ROOT)/daqmw
INSTALL_DIR  = $(DAQ_HTML_DIR)/js

INSTALL_FILES += daq.js
INSTALL_FILES += prototype.js

SUBDIRS +=

all: $(SUBDIRS)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

clean:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

install:
	install -d $(INSTALL_DIR)
	install -m 0644 $(INSTALL_FILES) $(INSTALL_DIR)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

uninstall:
	@for file in $(INSTALL_FILES); do rm -f $(INSTALL_DIR)/$${file}; done
