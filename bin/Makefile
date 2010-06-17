prefix = /usr

DIR = $(prefix)/bin
MODE = 0755

FILES += condition_xml2json
FILES += xml2json-with-attribute.xslt
FILES += genmake.py
FILES += run-comps.sh
FILES += run-local.py
FILES += run.py

all:

clean:

install:
	mkdir -p $(DIR)
	install -m $(MODE) $(FILES) $(DIR)

uninstall:
	@echo "---> uninstalling bin files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
