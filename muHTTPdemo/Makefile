###############################################################################
#	Makefile
###############################################################################

CPP_SOURCES=main.cpp webpages.cpp
TARGET_EXE=muHTTPdemo
TARGET_TYPE=exe

#	Tools

WX_CONFIG=wx-config
GCC=g++
LIBS=

ifdef DEBUG
	WX_CFLAGS=$(shell $(WX_CONFIG) --cflags --unicode=no --debug=yes)
	WX_LDFLAGS=$(shell $(WX_CONFIG) --libs --unicode=no --debug=yes)
	EXTERN_LIBS+=../libmuHTTPd/libmuhttpd_debug.a
else
	WX_CFLAGS=$(shell $(WX_CONFIG) --cflags --unicode=no --debug=no)
	WX_LDFLAGS=$(shell $(WX_CONFIG) --libs --unicode=no --debug=no)
	EXTERN_LIBS+=../libmuHTTPd/libmuhttpd_release.a
endif

INCLUDES=-I ../libmuHTTPd/
CFLAGS+=$(WX_CFLAGS) 
LIBS+=$(WX_LDFLAGS) 

include ../build/buildsys.mk

#	Dependant libraries

../libmuHTTPd/libmuhttpd_release.a: .PHONY
	+@make -C ../libmuHTTPd

../libmuHTTPd/libmuhttpd_debug.a: .PHONY
	+@make DEBUG=1 -C ../libmuHTTPd

# DO NOT DELETE

$(OBJ_DIR)/main.o: muHTTPdemoApp.h ../libmuHTTPd/dbgutils.h
$(OBJ_DIR)/main.o: ../libmuHTTPd/serverClasses.h ../libmuHTTPd/mustylesheet.h
$(OBJ_DIR)/main.o: webpages.h ../libmuHTTPd/muhttpd.h ../libmuHTTPd/mutable.h
$(OBJ_DIR)/webpages.o: ../libmuHTTPd/serverClasses.h
$(OBJ_DIR)/webpages.o: ../libmuHTTPd/mustylesheet.h ../libmuHTTPd/muhttpd.h
$(OBJ_DIR)/webpages.o: ../libmuHTTPd/mutable.h ../libmuHTTPd/muhtmlhelpers.h
$(OBJ_DIR)/webpages.o: ../libmuHTTPd/muutils.h ../libmuHTTPd/dbgutils.h
$(OBJ_DIR)/webpages.o: image/html_02_00_jpg.h image/html_debuggerfe_ico.h
