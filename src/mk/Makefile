prefix = /usr

DIR = $(prefix)/share/daqmw/mk
MODE = 0644

FILES += comp.mk

all:

install:
	mkdir -p $(DIR)
	install -m $(MODE) $(FILES) $(DIR)

clean:

uninstall:
	@echo "---> uninstalling mk files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
