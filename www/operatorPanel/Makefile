
# Change SOCK_DIR and SOCK_LIB as you need, please!
# SOCK_DIR: directory which has Sock.h and libSock.a.

#SOCK_DIR      = /home/nakayosi/work/Socket/C++

SOCK_DIR       = ../../SiTCP/CPP/Sock
SOCK_LIB       = ${SOCK_DIR}/libSock.a

# You don't have to change followings.

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
	${CXX} ${CXX_FLAGS} -I${SOCK_DIR} -I${PYTHON_INC_DIR} ${WRAPFILE}
	${CXX} -shared ${WRAPFILE_O} ${SOCK_LIB} -o ${SHAREDLIB}
#${CXX} -shared ${WRAPFILE_O} -o ${SHAREDLIB}

clean:
	rm -f ${IFILE}.py ${WRAPFILE} ${WRAPFILE_O} ${SHAREDLIB}
