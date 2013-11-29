include ../../www/Distributions.mk

DESTDIR =
prefix  = /usr
DIR     = $(DESTDIR)$(prefix)/bin

PROG = dmbepanel.py

all: $(PROG)

$(PROG): $(PROG).in
ifeq ($(strip $(USE_MOD_WSGI)),1)
	@sed 's|@URL@|"http://localhost/daqmw/scripts/daq.py/"|' $(PROG).in > $(PROG)
endif
ifeq ($(strip $(USE_MOD_PYTHON)),1)
	@sed 's|@URL@|"http://localhost/daqmw/operatorPanel/daq.py/"|' $(PROG).in > $(PROG)
endif

install:
	@mkdir -p $(DIR)
	@install -m 755 $(PROG) $(DIR)

uninstall:
	@rm -f $(DIR)/$(PROG)

clean:
	@rm -f $(PROG)
