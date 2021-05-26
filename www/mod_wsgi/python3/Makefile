include ../../../Distributions.mk

DESTDIR =
DAQ_CONF_DIR = $(DESTDIR)$(HTTPD_CONF_DIR)

INSTALL_FILES += daq.conf

SUBDIRS += js
SUBDIRS += operatorPanel
SUBDIRS += scripts
SUBDIRS += parts

.PHONY: subdirs $(SUBDIRS)

all: daq.conf $(SUBDIRS)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

daq.conf: daq.conf.in
	sed -e 's|@WWW_DOCUMENT_ROOT@|$(WWW_DOCUMENT_ROOT)|g' daq.conf.in > daq.conf

clean:
	rm -f daq.conf
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

install:
	install -d $(DAQ_CONF_DIR)
	install -m 0644 $(INSTALL_FILES) $(DAQ_CONF_DIR)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

uninstall:
	@for file in $(INSTALL_FILES); do rm -f $(DAQ_CONF_DIR)/$${file}; done
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
