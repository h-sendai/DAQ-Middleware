DESTDIR =
prefix  = /usr

SUBDIRS += idl

DIR = $(DESTDIR)$(prefix)/include/daqmw
MODE = 0644

FILES += Condition.h
FILES += DaqComponentBase.h
FILES += DaqComponentException.h
FILES += FatalType.h
FILES += Timer.h
FILES += json2conlist.h
FILES += DaqLog.h

all:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
	

install:
	mkdir -p $(DIR)
	install -m $(MODE) $(FILES) $(DIR)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

clean:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

uninstall:
	@echo "---> uninstalling DaqComponent files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
