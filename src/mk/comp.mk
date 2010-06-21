ifndef BASE_PATH
# not in source tree
BASE_PATH=/usr/include/daqmw
DAQCOMPONENT=
DAQSERVICE=idl
else
# in source tree
DAQCOMPONENT=DaqComponent
DAQSERVICE=DaqService
endif

#VPATH += $(BASE_PATH)/DaqComponent
#VPATH += $(BASE_PATH)/DaqService
##VPATH += ../../..

VPATH += $(BASE_PATH)/$(DAQCOMPONENT)
VPATH += $(BASE_PATH)/$(DAQSERVICE)

#CPPFLAGS += -I$(BASE_PATH)/DaqComponent
#CPPFLAGS += -I$(BASE_PATH)/DaqService

CPPFLAGS += -I$(BASE_PATH)/$(DAQCOMPONENT)
CPPFLAGS += -I$(BASE_PATH)/$(DAQSERVICE)

#CPPFLAGS += -I../../..
CPPFLAGS += -I.

OBJS     += $(subst .cpp,.o, $(SRCS))

CXXFLAGS += `rtm-config --cflags`
LDFLAGS  += `rtm-config --libs`
SHFLAGS  = -shared

IDLC     = `rtm-config --idlc`
IDLFLAGS = `rtm-config --idlflags` -I`rtm-config --prefix`/include/rtm/idl
WRAPPER  = rtm-skelwrapper
WRAPPER_FLAGS = --include-dir="" --skel-suffix=Skel --stub-suffix=Stub

SKEL_OBJ  = DAQServiceSkel.o  	
STUB_OBJ  = DAQServiceStub.o 
IMPL_OBJ  = DAQServiceSVC_impl.o 
OBJS     += $(SKEL_OBJ) $(IMPL_OBJ)
#OBJS     += $(COMP_NAME).o

# BINDIR = $(BASE_PATH)/../bin

symlink:
	rm -f DAQService.idl
	ln -s $(BASE_PATH)/$(DAQSERVICE)/DAQService.idl
	touch symlink

$(COMP_NAME).h: DAQServiceSVC_impl.h
$(COMP_NAME).o: $(COMP_NAME).h $(COMP_NAME).cpp

$(COMP_NAME)Comp: $(COMP_NAME)Comp.o $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

# install target should be defined in the user's makefile
# because we don't know where is the install directory.
# Please do not remove this comment out.
#install: all
#	mkdir -p $(BINDIR)
#	install -m 755 $(COMP_NAME)Comp $(BINDIR)

clean:
	@rm -f $(OBJS) $(COMP_NAME)Comp.o $(COMP_NAME)Comp
	@rm -f *Skel.h *Skel.cpp
	@rm -f *Stub.h *Stub.cpp
	@rm -f *~
	@rm -f DAQService.hh DAQServiceDynSK.cc DAQServiceSK.cc
	@rm -f DAQService.idl
	@rm -f symlink

# XXX: symlink
DAQService.idl: symlink

#DAQServiceSkel.cpp: symlink DAQService.idl
DAQServiceSkel.cpp: DAQService.idl
	$(IDLC) $(IDLFLAGS) DAQService.idl
	$(WRAPPER) $(WRAPPER_FLAGS) --idl-file=DAQService.idl
# XXX: symlink
#DAQServiceSkel.h : symlink DAQService.idl
DAQServiceSkel.h : DAQService.idl
	$(IDLC) $(IDLFLAGS) DAQService.idl
	$(WRAPPER) $(WRAPPER_FLAGS) --idl-file=DAQService.idl

#MlfComponent.h:       DaqComponentBase.h
#DaqComponentBase.h:   DAQServiceSVC_impl.h

DAQServiceSVC_impl.h: DAQServiceSkel.h

$(COMP_NAME).o:     $(COMP_NAME).h $(COMP_NAME).cpp 
$(COMP_NAME)Comp.o: $(COMP_NAME)Comp.cpp $(COMP_NAME).h

DAQServiceSVC_impl.o: DAQServiceSVC_impl.cpp DAQServiceSVC_impl.h DAQServiceSkel.h
DAQServiceSkel.o: DAQServiceSkel.cpp DAQServiceSkel.h

# comp.mk ends here

# DAQServiceStub.cpp : DAQService.idl
#	$(IDLC) $(IDLFLAGS) DAQService.idl
#	$(WRAPPER) $(WRAPPER_FLAGS) --idl-file=DAQService.idl
# DAQServiceStub.h : DAQService.idl
#	$(IDLC) $(IDLFLAGS) DAQService.idl
#	$(WRAPPER) $(WRAPPER_FLAGS) --idl-file=DAQService.idl

