# define prefix as a default value.
# argument in the make command (make install prefix=/usr) override this value.
prefix = /usr

LIBRARY    = lib$(LIBNAME).a
LIBRARY_SO = lib$(LIBNAME).so

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
	$(CC) -shared -o $(LIBRARY_SO) $(SHOBJS) $(CPPSHOBJS)

.SUFFIXES: .so
.c.so:
	rm -f $@
	$(CC) $(CPPFLAGS) -c ${CFLAGS} $(PIC_OPT) -o $*.so $< 
.cpp.so:
	rm -f $@
	$(CXX) $(CPPFLAGS) -c ${CXXFLAGS} $(PIC_OPT) -o $*.so $< 

clean:
	rm -f $(LIBRARY) $(OBJS) $(LIBRARY_SO) $(SHOBJS) $(CPPOBJS) $(CPPSHOBJS)

#INSTALL_LIB_DIR = $(prefix)/lib
#INSTALL_INC_DIR = $(prefix)/include

DAQMW_SUFFIX    = daqmw
INSTALL_LIB_DIR = $(prefix)/lib/$(DAQMW_SUFFIX)
INSTALL_INC_DIR = $(prefix)/include/$(DAQMW_SUFFIX)

install:
	mkdir -p $(INSTALL_LIB_DIR)
	install -m 0644 $(TARGET) $(INSTALL_LIB_DIR)
	mkdir -p $(INSTALL_INC_DIR)
	install -m 0644 $(API_INCLUDE_FILES) $(INSTALL_INC_DIR)

echo:
	@echo $(OBJS)
	@echo $(SHOBJS)
	@echo "CPPOBJS" $(CPPOBJS)

uninstall:
	for file in $(TARGET); do rm -f $(INSTALL_LIB_DIR)/$${file}; done
	for file in $(API_INCLUDE_FILES); do rm -f $(INSTALL_INC_DIR)/$${file}; done
