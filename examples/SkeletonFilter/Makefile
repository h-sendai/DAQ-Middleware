COMP_NAME = SkeletonFilter

all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp

# sample install target
#
# MODE = 0755
# BINDIR = /tmp/mybinary
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

ifdef DAQMWSRCROOT
include ${DAQMWSRCROOT}/src/mk/comp.mk
else
include /usr/share/daqmw/mk/comp.mk
endif
