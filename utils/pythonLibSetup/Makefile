include ../../Distributions.mk

DESTDIR = 
prefix  = /usr

# We may use "from __future__" on python3
PYTHON_SITELIB=$(shell $(PYTHON_EXEC_PATH) -c "from __future__ import print_function; from distutils.sysconfig import get_python_lib; print(get_python_lib())")

PYTHON_LIB_DIR        = $(DESTDIR)$(PYTHON_SITELIB)
PYTHON_LIB_DAQMW_DIR  = $(DESTDIR)$(PYTHON_SITELIB)/daqmw
PYTHON_PATH_FILE      = daqmw.pth
PYTHON_INIT_FILE      = __init__.py

LIB_MODE = 0644

all:

install: all
	@mkdir -p $(PYTHON_LIB_DAQMW_DIR)
	@install -m $(LIB_MODE) $(PYTHON_PATH_FILE) $(PYTHON_LIB_DIR)
	@install -m $(LIB_MODE) $(PYTHON_INIT_FILE) $(PYTHON_LIB_DAQMW_DIR)

uninstall:
# XXX
# Should I remove site-packages/daqmw.pth and site-packages/daqmw/__init__.py?

clean:
