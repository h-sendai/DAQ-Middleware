prefix = /usr

DIR = $(prefix)/include/daqmw
MODE = 0644

FILES += Condition.h
FILES += DaqComponentBase.h
FILES += DaqComponentException.h
FILES += FatalType.h
FILES += Timer.h
FILES += data_header_footer.h
FILES += json2conlist.h

all:

install:
	mkdir -p $(DIR)
	install -m $(MODE) $(FILES) $(DIR)

clean:

uninstall:
	@echo "---> uninstalling DaqComponent files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
