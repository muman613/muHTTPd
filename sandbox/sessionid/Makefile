###############################################################################
#	Makefile
###############################################################################

CPP_SOURCES=sessionid.cpp
TARGET_EXE=sessionid
TARGET_TYPE=exe

#	Tools

GCC=g++

SQLITE3_LIBS=$(shell pkg-config --libs sqlite3)

LIBS=$(SQLITE3_LIBS)

ifdef DEBUG
	EXE_DIR=bin/Debug
	OBJ_DIR=obj/Debug
	CFLAGS+= -D_DEBUG=1 -g3
else
	EXE_DIR=bin/Release
	OBJ_DIR=obj/Release
	CFLAGS+= -DNDEBUG -O2
endif

TARGET=$(EXE_DIR)/$(TARGET_EXE)


INCLUDES=

CFLAGS+=$(INCLUDES)
LDFLAGS=$(LIBS) $(EXTERN_LIBS)

include ../../build/buildsys.mk

# DO NOT DELETE
