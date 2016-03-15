##### @version $Id: Makefile 1108 2012-02-15 08:24:22Z liuzq $  #####
#######Makefile Begin#######################

TARGET_NAME := dllcommon

#Debug or Release
BUILD_TYPE := Debug

#dynamic_lib/static_lib/executable
MODULE_TYPE := dynamic_lib
COMMON_PATH := ../..

TARGET := $(TARGET_NAME)

SRCDIRS   := ./src
SRCDIRS   += 

CPPFLAGS  := -fPIC -I./src
CPPFLAGS  +=

LDFLAGS   := -lrt

BUILD_PRE : 
	rm -rf bin/$(BUILD_TYPE)/*
	mkdir -p bin/$(BUILD_TYPE)

BUILD_POST :
	rm -rf $(COMMON_PATH)/include/dllcommon
	@mkdir -p $(COMMON_PATH)/include/dllcommon
	cp -p ./src/*.h $(COMMON_PATH)/include/dllcommon/
	@mkdir -p $(COMMON_PATH)/lib/$(BUILD_TYPE)
	cp -p ./bin/$(BUILD_TYPE)/*.* $(COMMON_PATH)/lib/$(BUILD_TYPE)/

include ../../build/build_module.mk

### End of the Makefile 
###############################################################################

