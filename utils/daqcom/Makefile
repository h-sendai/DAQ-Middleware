DESTDIR = 
prefix  = /usr

PYTHON_SITELIB=$(shell python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()")

DAQCOM_DIR   = $(DESTDIR)$(PYTHON_SITELIB)/daqmw/daqcom
HTTPLIB2_DIR = $(DESTDIR)$(PYTHON_SITELIB)/daqmw/daqcom/httplib2

MODE      = 0644 
EXEC_MODE = 0755

DAQCOM_FILE          = daqcom.py
DAQCOM_EXEC_PATH = $(DESTDIR)$(prefix)/bin/daqcom

HTTPLIB2_FILES += httplib2/__init__.py
HTTPLIB2_FILES += httplib2/iri2uri.py

all:

install:
	@mkdir -p $(HTTPLIB2_DIR)
	@install -m $(EXEC_MODE) $(DAQCOM_FILE) $(DAQCOM_DIR)
	@install -m $(MODE) $(HTTPLIB2_FILES) $(HTTPLIB2_DIR)
	@ln -sf $(PYTHON_SITELIB)/daqmw/daqcom/$(DAQCOM_FILE) $(DAQCOM_EXEC_PATH)

uninstall:
	@rm -fr $(DAQCOM_DIR)
	@rm -f  $(DAQCOM_EXEC_PATH)

clean:
