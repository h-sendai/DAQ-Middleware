include ../../../../Distributions.mk

SWIG = $(shell which swig)
ifeq ($(strip $(SWIG)),)
$(error Cannot find swig. Please install swig program.\
Consult your distributions package.)
endif

DESTDIR           =
DAQ_HTML_DIR      = $(DESTDIR)$(WWW_DOCUMENT_ROOT)/daqmw
INSTALL_DIR       = $(DAQ_HTML_DIR)/scripts

INSTALL_FILES += _ParameterClient.so
INSTALL_FILES += ParameterClient.py
INSTALL_FILES += daq.py
INSTALL_FILES += writedata.py

SOCK_INC       = ../../../src/lib/SiTCP/CPP/Sock
SOCK_LIB       = -L../../../src/lib/SiTCP/CPP/Sock -lSock

SWIG_FLAGS     = -c++ -python
PYTHON_VERSION = $(shell python -c 'import sys; v = "%d.%d" % (sys.version_info[0], sys.version_info[1]); print v')
PYTHON_INC_DIR = $(shell ${PYTHON_CONFIG} --includes)

IFILE          = ParameterClient
SRCS           = ${IFILE}.h
INTERFACE      = $(IFILE).i
WRAPFILE       = $(IFILE)_wrap.cxx
WRAPFILE_O     = $(IFILE)_wrap.o
SHAREDLIB      = _${IFILE}.so
CXX_FLAGS      = -c -fpic

all: ${SHAREDLIB} writedata.html writedata.py

writedata.html: writedata.html.in
	sed -e 's|@WWW_DOCUMENT_ROOT@|$(WWW_DOCUMENT_ROOT)|' writedata.html.in > writedata.html

writedata.py: writedata.py.in
	sed -e 's|@WWW_DOCUMENT_ROOT@|$(WWW_DOCUMENT_ROOT)|' writedata.py.in > writedata.py

${SHAREDLIB}: ${SRCS}
	${SWIG} ${SWIG_FLAGS} ${INTERFACE}
	${CXX} ${CXX_FLAGS} -I${SOCK_INC} ${PYTHON_INC_DIR} ${WRAPFILE}
	${CXX} -shared ${WRAPFILE_O} ${SOCK_LIB} -o ${SHAREDLIB}

install:
	install -d -m 0755 $(INSTALL_DIR)
	install -m 0644 $(INSTALL_FILES) $(INSTALL_DIR)

clean:
	rm -f ${IFILE}.py *.pyc ${WRAPFILE} ${WRAPFILE_O} ${SHAREDLIB}
	rm -f writedata.html writedata.py

uninstall:
	@for file in $(INSTALL_FILES); do rm -f $(INSTALL_DIR)/$${file}; done
