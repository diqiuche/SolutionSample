##### @version $Id: Makefile 1084 2015-10-27  Eric $  #####
#######Makefile Begin#######################

SRCEXTS := .cpp
CC = g++
SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
OBJS= $(patsubst %.cpp,./obj/%.o,$(notdir $(SOURCES)))

vpath %.o   $(dir $(OBJS))
vpath %.cpp $(dir $(SOURCES))

#ifeq ($(BUILD_TYPE), Debug)
CPPFLAGS += -g
#endif

#add for futureapi define
#CPPFLAGS += -D AFW_CFG_LINUX
##

$(warning  $(OBJS))	
DEPS = $(patsubst %.o,%.d,$(OBJS))
-include $(DEPS)

.PHONY : all clean install
all : $(TARGET) BUILD_POST 

$(OBJS) :
./obj/%.o : %.cpp
	@mkdir -p $(dir $@)
	@echo "============="
	@echo "Compiling $<"
	$(CC) -c $(CPPFLAGS) $< -o $@

$(TARGET) : BUILD_PRE $(OBJS)
	@mkdir -p ./bin/$(BUILD_TYPE)

ifeq ($(MODULE_TYPE), dynamic_lib)
	$(CC) -o ./bin/$(BUILD_TYPE)/lib_$(TARGET_NAME).so -shared $(OBJS) $(STATIC_LIBS) $(LDFLAGS)
endif

ifeq ($(MODULE_TYPE), static_lib)
	ar rcs ./bin/$(BUILD_TYPE)/lib_$(TARGET_NAME).a $(OBJS)
endif

ifeq ($(MODULE_TYPE), executable)
	$(CC) -o ./bin/$(BUILD_TYPE)/exe_$(TARGET_NAME) $(OBJS) $(LDFLAGS)
endif

install : all
	@mkdir -p $(INSTALL_DIR)/appsample/bin
	@mkdir -p $(INSTALL_DIR)/appsample/lib
	@mkdir -p $(INSTALL_DIR)/appsample/etc

ifeq ($(MODULE_TYPE), dynamic_lib)
	cp -f ./bin/$(BUILD_TYPE)/lib_$(TARGET_NAME).so $(INSTALL_DIR)/appsample/lib
endif

ifeq ($(MODULE_TYPE), static_lib)
	cp -f ./bin/$(BUILD_TYPE)/lib_$(TARGET_NAME).a $(INSTALL_DIR)/appsample/lib 
endif

ifeq ($(MODULE_TYPE), executable)
	cp -f $(COMMON_PATH)/lib/Debug/*.so $(INSTALL_DIR)/appsample/lib
#	cp -f $(COMMON_PATH)/lib/Debug/*.a $(INSTALL_DIR)/appsample/lib
	cp -f $(COMMON_PATH)/contrib/libevent/lib/linux/*.so* $(INSTALL_DIR)/appsample/lib
	chmod a+x $(INSTALL_DIR)/appsample/lib/*.so
#	chmod a+x $(INSTALL_DIR)/appsample/lib/*.a
	cp -f ./bin/$(BUILD_TYPE)/exe_$(TARGET_NAME) $(INSTALL_DIR)/appsample/bin
#        ifneq  ($(wildcard *.conf), )
#		 #cp -f *.conf $(INSTALL_DIR)/appsample/etc
#        endif
    ifneq  ($(wildcard APPSTART*.sh), )
		cp -f APPSTART*.sh $(INSTALL_DIR)/appsample/bin
		chmod a+x $(INSTALL_DIR)/appsample/bin/APPSTART*.sh
    endif
    ifneq  ($(wildcard *.conf), )
		cp -f *.conf $(INSTALL_DIR)/appsample/etc
    endif
endif

clean :
	@echo "cleaning ..."
	rm -f $(OBJS)
	rm -f ./bin/$(BUILD_TYPE)/*

###############################################################################

