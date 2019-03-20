DESTDIR =
prefix  = /usr

DIR = $(DESTDIR)$(prefix)/share/daqmw/docs
MODE = 0644

FILES += README
FILES += README.json_spirit_v2.06
FILES += DAQ-Middleware-1.1.0-Tech.pdf
FILES += DAQ-Middleware-1.4.3-DevManual.pdf
FILES += Changes
FILES += Changes.ja
FILES += LICENSE
FILES += COPYING
FILES += COPYING.LESSER
FILES += VERSION

all:

clean:

install:
	mkdir -p $(DIR)
	install -m $(MODE) $(FILES) $(DIR)

uninstall:
	@echo "---> uninstalling conf files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
