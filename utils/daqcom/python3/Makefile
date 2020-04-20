include ../../../Distributions.mk

PYTHON_SITELIB=$(shell $(PYTHON_EXEC_FILE) -c "from distutils.sysconfig import get_python_lib; x = get_python_lib(); print(x)")
all:
	@echo "dummy Makefile in utils/daqcom/python3"
	@echo $(PYTHON_EXEC_FILE)
	@echo $(PYTHON_SITELIB)
