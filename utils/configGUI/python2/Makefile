DESTDIR = 
prefix  = /usr

PYTHON_SITELIB=$(shell python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()")

CONFIG_GUI_LIB_DIR  = $(DESTDIR)$(PYTHON_SITELIB)/daqmw/configGUI
CONFIG_GUI_EXEC_DIR = $(DESTDIR)$(prefix)/bin

LIB_MODE  = 0644 
EXEC_MODE = 0755

CONFIG_GUI_EXEC_FILE = confPanel.py
CONFIG_GUI_LIB_FILE  += __init__.py
CONFIG_GUI_LIB_FILE  += image_nb.py
CONFIG_GUI_LIB_FILE  += notebook.py
CONFIG_GUI_LIB_FILE  += writer.py
CONFIG_GUI_EXEC_PATH = $(CONFIG_GUI_EXEC_DIR)/$(CONFIG_GUI_EXEC_FILE)

all: confPanel.py

install: all
	@mkdir -p $(CONFIG_GUI_EXEC_DIR)
	@install -m $(EXEC_MODE) $(CONFIG_GUI_EXEC_FILE) $(CONFIG_GUI_EXEC_DIR)
	@mkdir -p $(CONFIG_GUI_LIB_DIR)
	@install -m $(LIB_MODE) $(CONFIG_GUI_LIB_FILE) $(CONFIG_GUI_LIB_DIR)
	@install -m $(LIB_MODE) $(CONFIG_GUI_LIB_FILE) $(CONFIG_GUI_LIB_DIR)

uninstall:
	@rm -f  $(CONFIG_GUI_EXEC_PATH)
	@rm -fr $(CONFIG_GUI_LIB_DIR)

confPanel.py: confPanel.py.in images/one.gif.base64 images/two.gif.base64 images/four.gif.base64
	@./rewrite.sh

images/one.gif.base64: images/one.gif
	@base64 $< > $@
images/two.gif.base64: images/two.gif
	@base64 $< > $@
images/four.gif.base64: images/four.gif
	@base64 $< > $@

clean:
	@rm -f confPanel.py *.pyc images/*.base64
