###############################################################################
#	Makefile
###############################################################################

#vpath %.cpp ../../libmuHTTPd

CPP_SOURCES=main.cpp mustylesheet.cpp
TARGET_EXE=css
TARGET_TYPE=exe

#	Tools

GCC=g++

WX_CFLAGS=$(shell wx-config --cflags)
WX_LIBS=$(shell wx-config --libs)

LIBS=$(WX_LIBS)
CFLAGS+=$(WX_CFLAGS)
INCLUDES=-I ../../libmuHTTPd


include ../../build/buildsys.mk

$(OBJ_DIR)/mustylesheet.o: ../../libmuHTTPd/mustylesheet.cpp
	@echo "Compiling $<"
	@$(GCC) -c -o $*.o $(CFLAGS) $<

# DO NOT DELETE

$(OBJ_DIR)/main.o: ../../libmuHTTPd/dbgutils.h
$(OBJ_DIR)/main.o: ../../libmuHTTPd/mustylesheet.h
