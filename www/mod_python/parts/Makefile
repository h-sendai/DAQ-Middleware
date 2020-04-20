include ../../../Distributions.mk

DESTDIR =
DAQ_HTML_DIR = $(DESTDIR)$(WWW_DOCUMENT_ROOT)/daqmw
INSTALL_DIR  = $(DAQ_HTML_DIR)/parts

INSTALL_FILES += beginbutton.jpg
INSTALL_FILES += beginbuttonlight.jpg
INSTALL_FILES += configurationbutton.jpg
INSTALL_FILES += configurationbuttonlight.jpg
INSTALL_FILES += endbutton.jpg
INSTALL_FILES += endbuttonlight.jpg
INSTALL_FILES += getstatebutton.jpg
INSTALL_FILES += pausebutton.jpg
INSTALL_FILES += pausebuttonlight.jpg
INSTALL_FILES += restartbutton.jpg
INSTALL_FILES += restartbuttonlight.jpg
INSTALL_FILES += setstatebutton.jpg
INSTALL_FILES += unconfigurebutton.jpg
INSTALL_FILES += unconfigurebuttonlight.jpg

SUBDIRS +=

all: $(SUBDIRS)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

clean:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

install:
	install -d $(INSTALL_DIR)
	install -m 0644 $(INSTALL_FILES) $(INSTALL_DIR)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

uninstall:
	@for file in $(INSTALL_FILES); do rm -f $(INSTALL_DIR)/$${file}; done
