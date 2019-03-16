DESTDIR = 
prefix  = /usr

DIR = $(DESTDIR)$(prefix)/bin
MODE = 0755

FILES += condition_xml2json
FILES += condition_xml2json.py
FILES += xml2json-with-attribute.xslt
FILES += run.py
FILES += run3.py
FILES += daqmw-rpm

all:

clean:

install:
	mkdir -p $(DIR)
	install -m $(MODE) $(FILES) $(DIR)

uninstall:
	@echo "---> uninstalling bin files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
