LIBNAME = JsonSpirit
TARGET  = lib$(LIBNAME).a lib$(LIBNAME).so
API_VERSION = 1
PATCHLEVEL  = 0

all: $(TARGET)

INC_DIRS += .

CPPSRCS += json_spirit_reader.cpp
CPPSRCS += json_spirit_value.cpp
CPPSRCS += json_spirit_writer.cpp

API_INCLUDE_FILES += json_spirit.h
API_INCLUDE_FILES += json_spirit_reader.h
API_INCLUDE_FILES += json_spirit_value.h
API_INCLUDE_FILES += json_spirit_writer.h

include ../../lib.mk
