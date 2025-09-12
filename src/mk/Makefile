include ../../Distributions.mk

DESTDIR =
prefix  = /usr

DIR = $(DESTDIR)$(prefix)/share/daqmw/mk
MODE = 0644

ifeq ($(strip $(USE_OPENRTM_AIST_VERSION)),1)
RTM_CONFIG=rtm-config
RTM_SKELWRAPPER=rtm-skelwrapper
endif

ifeq ($(strip $(USE_OPENRTM_AIST_VERSION)),2)
RTM_CONFIG=rtm2-config
RTM_SKELWRAPPER=rtm2-skelwrapper
endif

FILES += comp.mk

all: comp.mk

comp.mk: comp.mk.in
	sed -e 's|@RTM_CONFIG@|$(RTM_CONFIG)|g' -e 's|@RTM_SKELWRAPPER@|$(RTM_SKELWRAPPER)|g' comp.mk.in > comp.mk

install:
	mkdir -p $(DIR)
	install -m $(MODE) $(FILES) $(DIR)

clean:
	rm -f $(FILES)

uninstall:
	@echo "---> uninstalling mk files."
	@for file in $(FILES); do rm -f $(DIR)/$${file}; done
	@echo "---> done"
