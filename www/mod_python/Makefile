DESTDIR =
HTTPD_CONF_DIR = $(DESTDIR)/etc/httpd/conf.d

INSTALL_FILES += daq.conf

SUBDIRS += js
SUBDIRS += operatorPanel
SUBDIRS += parts

.PHONY: subdirs $(SUBDIRS)

#all: $(SUBDIRS)
#
#$(SUBDIRS):
#	@$(MAKE) -C $@

all: $(SUBDIRS)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

clean:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

install:
	install -d $(HTTPD_CONF_DIR)
	install -m 0644 $(INSTALL_FILES) $(HTTPD_CONF_DIR)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

uninstall:
	@for file in $(INSTALL_FILES); do rm -f $(HTTPD_CONF_DIR)/$${file}; done
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

#install:
#	install -m 0644 daq.conf /etc/httpd/conf.d/
#	@$(MAKE) -C operatorPanel install
#	cp -r js /var/www/html/daqmw/
#	cp -r parts /var/www/html/daqmw/
#clean:
#	@$(MAKE) -C operatorPanel clean
