include ../../Distributions.mk

DESTDIR =
prefix  = /usr

DIR = $(DESTDIR)$(prefix)/share/daqmw/etc/remote-boot
MODE = 0644
EXEC_MODE = 0755

FILES      += bootComps-xinetd.sample
FILES      += services.sample
FILES      += bootComps.socket.sample
FILES      += bootComps@.service.sample
FILES      += README
# EXEC_FILES += bootComps.py

all:

clean:

install:
	@mkdir -p $(DIR)
	@install -m $(MODE)      $(FILES)      $(DIR)
#	@install -m $(EXEC_MODE) $(EXEC_FILES) $(DIR)
ifeq ($(strip $(USE_PYTHON3)),1)
	install -m $(EXEC_MODE) bootComps.py3 $(DIR)/bootComps.py
endif
ifeq ($(strip $(USE_PYTHON2)),1)
	install -m $(EXEC_MODE) bootComps.py  $(DIR)/bootComps.py
endif


uninstall:
	@echo "---> uninstalling conf files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
#	@for file in $(EXEC_FILES); do rm -f $(DIR)/$${file}; done
	rm -f $(DIR)/bootComps.py
	@echo "---> done"
