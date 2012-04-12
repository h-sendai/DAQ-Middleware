include ../../Distributions.mk

DESTDIR           =
#WWW_DOCUMENT_ROOT = /var/www/html
DAQ_HTML_DIR      = $(DESTDIR)$(WWW_DOCUMENT_ROOT)/daqmw
INSTALL_DIR       = $(DAQ_HTML_DIR)/operatorPanel

INSTALL_FILES += operatorPanel0.html
INSTALL_FILES += runNumber.txt
INSTALL_FILES += style.xsl

all:

install:
	install -d -m 0755 $(INSTALL_DIR)
	install -m 0644 $(INSTALL_FILES) $(INSTALL_DIR)
	chmod 0666 $(INSTALL_DIR)/runNumber.txt

clean:


uninstall:
	@for file in $(INSTALL_FILES); do rm -f $(INSTALL_DIR)/$${file}; done
