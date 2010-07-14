prefix = /usr

DESTDIR = $(prefix)/share/daqmw/examples
#MODE = 0644

SRC_DIRS += BestEffortDispatcher
SRC_DIRS += Dispatcher
SRC_DIRS += EchoMonitor
SRC_DIRS += EchoReader
SRC_DIRS += Logger
SRC_DIRS += SampleMonitor
SRC_DIRS += SampleReader
SRC_DIRS += Skeleton
SRC_DIRS += SkeletonFilter
SRC_DIRS += SkeletonSink
SRC_DIRS += SkeletonSource

all:

clean:

install:
	@mkdir -p $(DESTDIR)
	@set -e; for dir in $(SRC_DIRS); do cp -r $${dir} $(DESTDIR); done

uninstall:
	@echo "---> uninstalling example files."
	@for dir in $(SRC_DIRS); do rm -fr $(DESTDIR)/$${dir}; done
	@echo "---> done"
