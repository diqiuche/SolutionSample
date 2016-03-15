##### @version $Id: Makefile 1084 2012-02-15 08:10:28Z liuzq $  #####
#######Makefile Begin#######################

TARGET_NAME := BasePlatform

#Debug or Release
BUILD_TYPE := Debug

#dynamic_lib/static_lib/executable
MODULE_TYPE := executable
COMMON_PATH := ../..

TARGET := $(TARGET_NAME)

SRCDIRS   := ./src/projectframe ./src/samplecode ./src/task
SRCDIRS   += 

CPPFLAGS  := -I$(COMMON_PATH)/include/dllcommon -I$(COMMON_PATH)/include/LibeventTcpServer -I$(COMMON_PATH)/contrib/libevent/include/linux -I./src/projectframe -I./src/samplecode -I./src/task
CPPFLAGS  += -std=c++0x

LDFLAGS   := -L$(COMMON_PATH)/lib/$(BUILD_TYPE)/ -L$(COMMON_PATH)/contrib/libevent/lib/linux -l_dllcommon -l_LibeventTcpServer -levent

BUILD_PRE : 
	rm -rf bin/$(BUILD_TYPE)/*

BUILD_POST :
	
include ../../build/build_module.mk

### End of the Makefile 
###############################################################################

