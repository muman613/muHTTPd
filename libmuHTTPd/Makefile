###############################################################################
#	Makefile
###############################################################################

CPP_SOURCES=muhtmlhelpers.cpp muhttpd.cpp mustylesheet.cpp \
            mutable.cpp muutils.cpp serverClasses.cpp

ifdef DEBUG
CPP_SOURCES+=dbgutils.cpp
endif

#	Tools

#WX_CONFIG=wx-config
GCC=gcc
AR=ar

TARGET_TYPE=statlib

ifdef DEBUG
LIBNAME=libmuhttpd_debug
else
LIBNAME=libmuhttpd_release
endif

include ../build/wxwidgets_env.mk

CFLAGS+=$(WX_CFLAGS)

include ../build/tools_env.mk
include ../build/buildsys.mk


# DO NOT DELETE

$(OBJ_DIR)/muhtmlhelpers.o: muhtmlhelpers.h
$(OBJ_DIR)/muhttpd.o: muhttpd.h serverClasses.h mustylesheet.h dbgutils.h
$(OBJ_DIR)/mustylesheet.o: mustylesheet.h dbgutils.h
$(OBJ_DIR)/mutable.o: mutable.h dbgutils.h
$(OBJ_DIR)/muutils.o: muutils.h dbgutils.h
$(OBJ_DIR)/serverClasses.o: serverClasses.h mustylesheet.h muutils.h
$(OBJ_DIR)/serverClasses.o: dbgutils.h

