DESTDIR =
prefix  = /usr

INSTALL_DIR = $(DESTDIR)$(prefix)/share/daqmw/examples
#MODE = 0644

SRC_DIRS += SampleLogger
SRC_DIRS += SampleMonitor
SRC_DIRS += SampleReader
SRC_DIRS += SampleReader2
SRC_DIRS += SampleFilter
SRC_DIRS += Skeleton
SRC_DIRS += SkeletonSink
SRC_DIRS += SkeletonSource
SRC_DIRS += ConditionSampleMonitor
SRC_DIRS += TinySink
SRC_DIRS += TinySource
SRC_DIRS += Dispatcher
SRC_DIRS += BestEffortDispatcher
SRC_DIRS += change-SampleComp-name

all:

clean:

install:
	@mkdir -p $(INSTALL_DIR)
	@set -e; for dir in $(SRC_DIRS); do cp -r $${dir} $(INSTALL_DIR); done

uninstall:
	@echo "---> uninstalling example files."
	@for dir in $(SRC_DIRS); do rm -fr $(INSTALL_DIR)/$${dir}; done
	@echo "---> done"
