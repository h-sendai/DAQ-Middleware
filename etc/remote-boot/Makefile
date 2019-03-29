DESTDIR =
prefix  = /usr

DIR = $(DESTDIR)$(prefix)/share/daqmw/etc/remote-boot
MODE = 0644
EXEC_MODE = 0755

FILES      += bootComps-xinetd.sample
FILES      += services.sample
EXEC_FILES += bootComps.py

all:

clean:

install:
	@mkdir -p $(DIR)
	@install -m $(MODE)      $(FILES)      $(DIR)
	@install -m $(EXEC_MODE) $(EXEC_FILES) $(DIR)

uninstall:
	@echo "---> uninstalling conf files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@for file in $(EXEC_FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
