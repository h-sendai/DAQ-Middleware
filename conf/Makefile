DESTDIR =
prefix  = /usr

DIR = $(DESTDIR)$(prefix)/share/daqmw/conf
MODE = 0644

FILES += condition.xml
FILES += config.xsd
FILES += sample.xml
FILES += skel.xml
FILES += tiny.xml
FILES += 4comps.xml
FILES += reader-logger.xml

all:

clean:

install:
	mkdir -p $(DIR)
	install -m $(MODE) $(FILES) $(DIR)

uninstall:
	@echo "---> uninstalling conf files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
