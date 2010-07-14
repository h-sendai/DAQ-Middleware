prefix = /usr

DIR = $(prefix)/share/daqmw/conf
MODE = 0644

FILES += condition.xml
FILES += config.xsd
FILES += example1.xml
FILES += example2.xml
FILES += example3.xml
FILES += example4.xml
FILES += myconfig.xml
FILES += test-cond.xml
FILES += test-mlf.xml
FILES += sample.xml

all:

clean:

install:
	mkdir -p $(DIR)
	install -m $(MODE) $(FILES) $(DIR)

uninstall:
	@echo "---> uninstalling conf files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
