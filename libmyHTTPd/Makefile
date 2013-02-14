###############################################################################
#	Makefile
###############################################################################

CPP_SOURCES=myhtmlhelpers.cpp myhttpd.cpp mystylesheet.cpp \
            mytable.cpp myutils.cpp serverClasses.cpp

#	Tools

WX_CONFIG=/opt/wxWidgets/2.8.12/bin/wx-config
GCC=gcc
AR=ar

ifdef DEBUG
	TARGET=libmyhttpd_debug.a
	CFLAGS+= -D_DEBUG=1 -g
	WX_CFLAGS=$(shell $(WX_CONFIG) --cflags --unicode=no --debug=yes)
	OBJ_DIR=obj/Debug
else
	TARGET=libmyhttpd_release.a
	WX_CFLAGS=$(shell $(WX_CONFIG) --cflags --unicode=no --debug=no)
	OBJ_DIR=obj/Release
endif

CFLAGS+=$(WX_CFLAGS)
CPP_OBJS=$(CPP_SOURCES:%.cpp=$(OBJ_DIR)/%.o)
OBJS=$(CPP_OBJS)

#	Default rule
$(OBJ_DIR)/%.o : %.cpp Makefile
	@echo "Compiling $<"
	@$(GCC) -c -o $(OBJ_DIR)/$*.o $(CFLAGS) $*.cpp

$(TARGET): $(OBJ_DIR) $(OBJS)
	@echo "Generating static library $(TARGET)"
	@$(AR) -r -s $(TARGET) $(OBJS)

clean:	.PHONY
	rm -rf $(OBJS) $(TARGET)

$(OBJ_DIR): .PHONY
	mkdir -p $(OBJ_DIR)
	
.PHONY:
	@true

# DO NOT DELETE

$(OBJ_DIR)/myhtmlhelpers.o: myhtmlhelpers.h
$(OBJ_DIR)/myhttpd.o: myhttpd.h serverClasses.h mystylesheet.h dbgutils.h
$(OBJ_DIR)/mystylesheet.o: mystylesheet.h dbgutils.h
$(OBJ_DIR)/mytable.o: mytable.h dbgutils.h
$(OBJ_DIR)/myutils.o: myutils.h dbgutils.h
$(OBJ_DIR)/serverClasses.o: serverClasses.h mystylesheet.h myutils.h
$(OBJ_DIR)/serverClasses.o: dbgutils.h

