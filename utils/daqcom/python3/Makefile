include ../../../Distributions.mk

DESTDIR = 
prefix  = /usr

#PYTHON_SITELIB=$(shell $(PYTHON_EXEC_PATH) -c "from distutils.sysconfig import get_python_lib; x = get_python_lib(); print(x)")

DAQCOM_DIR      = $(DESTDIR)$(PYTHON_SITELIB)/daqmw
HTTPLIB2_DIR    = $(DESTDIR)$(PYTHON_SITELIB)/daqmw/httplib2
DAQCOM_EXEC_DIR = $(DESTDIR)$(prefix)/bin

LIB_MODE  = 0644 
EXEC_MODE = 0755

DAQCOM_EXEC_FILE = daqcom
DAQCOM_LIB_FILE  = daqmwcom.py
DAQCOM_EXEC_PATH = $(DAQCOM_EXEC_DIR)/daqcom

HTTPLIB2_FILES += httplib2/__init__.py
HTTPLIB2_FILES += httplib2/iri2uri.py
HTTPLIB2_FILES += httplib2/cacerts.txt
HTTPLIB2_FILES += httplib2/socks.py
HTTPLIB2_FILES += httplib2/certs.py

all:

install:
	@mkdir -p $(DAQCOM_EXEC_DIR)
	@install -m $(EXEC_MODE) $(DAQCOM_EXEC_FILE) $(DAQCOM_EXEC_DIR)
	@mkdir -p $(HTTPLIB2_DIR)
	@install -m $(LIB_MODE) $(DAQCOM_LIB_FILE) $(DAQCOM_DIR)
	@install -m $(LIB_MODE) $(HTTPLIB2_FILES) $(HTTPLIB2_DIR)
	@mkdir -p $(DESTDIR)$(PYTHON_SITELIB)

uninstall:
	@rm -f  $(DAQCOM_EXEC_PATH)
	@rm -fr $(HTTPLIB2_DIR)
	@rm $(DAQCOM_DIR)/$(DAQCOM_LIB_FILE)

clean:
