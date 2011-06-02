DESTDIR =
prefix  = /usr

DIR = $(DESTDIR)$(prefix)/share/daqmw/etc/remote-boot
MODE = 0644

FILES += bootComps-xinetd.sample
FILES += bootComps.py
FILES += services.sample

all:

clean:

install:
	@mkdir -p $(DIR)
	@install -m $(MODE) $(FILES) $(DIR)

uninstall:
	@echo "---> uninstalling conf files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
