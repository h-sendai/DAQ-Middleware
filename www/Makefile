SUBDIRS += operatorPanel

.PHONY: subdirs $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@

install:
	install -m 0644 daq.conf /etc/httpd/conf.d/
	@$(MAKE) -C operatorPanel install
	cp -r js /var/www/html/daqmw/
	cp -r parts /var/www/html/daqmw/
clean:
	@$(MAKE) -C operatorPanel clean
