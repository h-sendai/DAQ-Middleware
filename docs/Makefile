prefix = /usr

DIR = $(prefix)/share/daqmw/docs
MODE = 0644

FILES += README
FILES += DAQ-Middleware-1.0.0-Tech.pdf

all:

clean:

install:
	mkdir -p $(DIR)
	install -m $(MODE) $(FILES) $(DIR)

uninstall:
	@echo "---> uninstalling conf files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
