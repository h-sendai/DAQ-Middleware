DESTDIR =
prefix  = /usr

DIR = $(DESTDIR)$(prefix)/share/daqmw/packages/rpm
MODE = 0644
EXEC_MODE = 0755

FILES      += DAQ-Middleware.spec

all:

clean:

install:
	@mkdir -p $(DIR)
	@install -m $(MODE)      $(FILES)      $(DIR)
	@install -m $(EXEC_MODE) $(EXEC_FILES) $(DIR)

uninstall:
	@echo "---> uninstalling conf files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
