
INSTALL_DIR = /var/www/html/daqmw/operatorPanel
LIB_DIR=lib
try_lib64 = $(shell ls -d /usr/lib64 2>/dev/null)
arch = $(shell uname -m)
ifeq "$(strip $(try_lib64))" "/usr/lib64"
ifeq "$(strip $(arch))"      "x86_64"
LIB_DIR=lib64
endif
endif

SOCK_INC       = ../../src/lib/SiTCP/CPP/Sock
SOCK_LIB       = -L../../src/lib/SiTCP/CPP/Sock -lSock

SWIG           = /usr/bin/swig
SWIG_FLAGS     = -c++ -python
PYTHON_VERSION = 2.4
PYTHON_INC_DIR = /usr/include/python${PYTHON_VERSION}

IFILE          = ParameterClient
SRCS           = ${IFILE}.h
INTERFACE      = $(IFILE).i
WRAPFILE       = $(IFILE)_wrap.cxx
WRAPFILE_O     = $(IFILE)_wrap.o
SHAREDLIB      = _${IFILE}.so
CXX_FLAGS      = -c -fpic

all: ${SHAREDLIB}

${SHAREDLIB}: ${SRCS}
	${SWIG} ${SWIG_FLAGS} ${INTERFACE}
	${CXX} ${CXX_FLAGS} -I${SOCK_INC} -I${PYTHON_INC_DIR} ${WRAPFILE}
	${CXX} -shared ${WRAPFILE_O} ${SOCK_LIB} -o ${SHAREDLIB}

install:
	mkdir -p $(INSTALL_DIR)
	install -m 0644 _ParameterClient.so $(INSTALL_DIR)
	install -m 0644 operatorPanel0.html $(INSTALL_DIR)
	install -m 0644 ParameterClient.py $(INSTALL_DIR)
	install -m 0644 daq.py $(INSTALL_DIR)
	install -m 0644 writedata.py $(INSTALL_DIR)
	install -m 0666 runNumber.txt $(INSTALL_DIR)

clean:
	rm -f ${IFILE}.py ${WRAPFILE} ${WRAPFILE_O} ${SHAREDLIB}
