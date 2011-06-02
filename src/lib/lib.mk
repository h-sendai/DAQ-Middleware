# define prefix as a default value.
# argument in the make command (make install prefix=/usr) override this value.
DESTDIR =
prefix  = /usr

LIBRARY                   = lib$(LIBNAME).a
LIBRARY_SO                = lib$(LIBNAME).so
LIBRARY_SO_API            = $(LIBRARY_SO).$(API_VERSION)
LIBRARY_SO_API_PATCHLEVEL = $(LIBRARY_SO).$(API_VERSION).$(PATCHLEVEL)

CFLAGS   = -g -pipe -O2 -Wall
CXXFLAGS = $(CFLAGS)
CPPFLAGS = $(addprefix -I, $(INC_DIRS))
ARFLAGS  = r
PIC_OPT  = -fPIC

# generate OBJS automatically.  Do not put spaces between ".c" and ".o"
OBJS   = $(subst .c,.o, $(SRCS))
SHOBJS = $(subst .c,.so, $(SRCS))

CPPOBJS   = $(subst .cpp,.o, $(CPPSRCS))
CPPSHOBJS = $(subst .cpp,.so, $(CPPSRCS))

$(LIBRARY): $(OBJS) $(CPPOBJS)
	$(AR) $(ARFLAGS) $@ $^

$(LIBRARY_SO): $(SHOBJS) $(CPPSHOBJS)
#	gcc -shared -Wl,-soname,libfoo.so.1 -o libfoo.so.1.0 *.o
	$(CC) -shared -Wl,-soname,$(LIBRARY_SO_API) -o $(LIBRARY_SO_API_PATCHLEVEL) $(SHOBJS) $(CPPSHOBJS)
	rm -f $(LIBRARY_SO) $(LIBRARY_SO_API)
	ln -s $(LIBRARY_SO_API) $(LIBRARY_SO)
	ln -s $(LIBRARY_SO_API_PATCHLEVEL) $(LIBRARY_SO_API)

.SUFFIXES: .so
.c.so:
	rm -f $@
	$(CC) $(CPPFLAGS) -c ${CFLAGS} $(PIC_OPT) -o $*.so $< 
.cpp.so:
	rm -f $@
	$(CXX) $(CPPFLAGS) -c ${CXXFLAGS} $(PIC_OPT) -o $*.so $< 

clean:
	rm -f $(LIBRARY) $(OBJS) $(LIBRARY_SO)* $(SHOBJS) $(CPPOBJS) $(CPPSHOBJS)

#INSTALL_LIB_DIR = $(prefix)/lib
#INSTALL_INC_DIR = $(prefix)/include

LIB_DIR=lib
try_lib64 = $(shell ls -d /usr/lib64 2>/dev/null)
arch = $(shell uname -m)
ifeq "$(strip $(try_lib64))" "/usr/lib64"
ifeq "$(strip $(arch))"      "x86_64"
LIB_DIR=lib64
endif
endif
DAQMW_SUFFIX    = daqmw
INSTALL_LIB_DIR = $(DESTDIR)$(prefix)/$(LIB_DIR)/$(DAQMW_SUFFIX)
INSTALL_INC_DIR = $(DESTDIR)$(prefix)/include/$(DAQMW_SUFFIX)

install:
	mkdir -p $(INSTALL_LIB_DIR)
	install -m 0644 $(LIBRARY) $(LIBRARY_SO_API_PATCHLEVEL) $(INSTALL_LIB_DIR)
	rm -f $(INSTALL_LIB_DIR)/$(LIBRARY_SO_API)
	rm -f $(INSTALL_LIB_DIR)/$(LIBRARY_SO)
	ln -s $(LIBRARY_SO_API_PATCHLEVEL) $(INSTALL_LIB_DIR)/$(LIBRARY_SO_API)
	ln -s $(LIBRARY_SO_API)            $(INSTALL_LIB_DIR)/$(LIBRARY_SO)
	mkdir -p $(INSTALL_INC_DIR)
	install -m 0644 $(API_INCLUDE_FILES) $(INSTALL_INC_DIR)
# shared library symlink creation
# 1. for ld
# ln -s libfoo.so.1 libfoo.so
# 2. for ld.so
# ln -s libfoo.so.1.0 libfoo.so.1 

echo:
	@echo $(OBJS)
	@echo $(SHOBJS)
	@echo "CPPOBJS" $(CPPOBJS)

uninstall:
	for file in $(TARGET); do rm -f $(INSTALL_LIB_DIR)/$${file}*; done
	for file in $(API_INCLUDE_FILES); do rm -f $(INSTALL_INC_DIR)/$${file}; done
