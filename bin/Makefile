include ../Distributions.mk

DESTDIR = 
prefix  = /usr

DIR = $(DESTDIR)$(prefix)/bin
MODE = 0755

FILES += condition_xml2json
FILES += xml2json-with-attribute.xslt
FILES += daqmw-rpm 

ifeq ($(strip $(USE_PYTHON2)),1)
    FILES += condition_xml2json.py
endif

all:

clean:

install:
	mkdir -p $(DIR)
	install -m $(MODE) $(FILES) $(DIR)
ifeq ($(strip $(USE_PYTHON3)),1)
	install -m $(MODE) run.py3 $(DIR)/run.py
endif
ifeq ($(strip $(USE_PYTHON2)),1)
	install -m $(MODE) run.py  $(DIR)/run.py
endif


uninstall:
	@echo "---> uninstalling bin files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	rm -f $(DIR)/run.py
	@echo "---> done"
