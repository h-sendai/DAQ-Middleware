COMP_NAME = SampleLogger

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp
SRCS += FileUtils.cpp

LDLIBS += -lboost_filesystem -lboost_date_time

# We have to link libboost_system if Boost version is 1.35 or later
BOOST_VERSION_FILE=/usr/include/boost/version.hpp
BOOST_VERSION=$(shell awk '/^\#define BOOST_VERSION / {print $$3}' $(BOOST_VERSION_FILE))
NEED_BOOST_SYSTEM_LIB_SINCE=103500
ifeq ($(shell echo "$(BOOST_VERSION) >= $(NEED_BOOST_SYSTEM_LIB_SINCE)" | bc), 1)
LDLIBS += -lboost_system
endif

# sample install target
#
# MODE = 0755
# BINDIR = /home/daq/bin
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

include /usr/share/daqmw/mk/comp.mk
