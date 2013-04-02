###############################################################################
#	Makefile
###############################################################################

CPP_SOURCES=main.cpp webpages.cpp

#	Tools

WX_CONFIG=wx-config
GCC=g++
AR=ar

LIBS=

ifdef DEBUG
	EXE_DIR=bin/Debug
	CFLAGS+= -D_DEBUG=1 -g
	WX_CFLAGS=$(shell $(WX_CONFIG) --cflags --unicode=no --debug=yes)
	WX_LDFLAGS=$(shell $(WX_CONFIG) --libs --unicode=no --debug=yes)
	OBJ_DIR=obj/Debug
	EXTERN_LIBS+=../libmuHTTPd/libmuhttpd_debug.a
else
	EXE_DIR=bin/Release
	WX_CFLAGS=$(shell $(WX_CONFIG) --cflags --unicode=no --debug=no)
	WX_LDFLAGS=$(shell $(WX_CONFIG) --libs --unicode=no --debug=no)
	OBJ_DIR=obj/Release
	EXTERN_LIBS+=../libmuHTTPd/libmuhttpd_release.a
endif

TARGET=$(EXE_DIR)/muHTTPdemo

INCLUDES=-I ../libmuHTTPd/
CFLAGS+=$(WX_CFLAGS) $(INCLUDES)
LDFLAGS=$(LIBS) $(EXTERN_LIBS) $(WX_LDFLAGS) 

CPP_OBJS=$(CPP_SOURCES:%.cpp=$(OBJ_DIR)/%.o)
OBJS=$(CPP_OBJS)

#	Default rule
$(OBJ_DIR)/%.o : %.cpp Makefile
	@echo "Compiling $*.cpp"
	@$(GCC) -c -o $(OBJ_DIR)/$*.o $(CFLAGS) $*.cpp

$(TARGET): $(OBJ_DIR) $(EXE_DIR) $(OBJS) $(EXTERN_LIBS)
	@echo "Linking $(TARGET)"
	@$(GCC) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:	.PHONY
	rm -rf $(OBJS) $(TARGET)

$(OBJ_DIR) : .PHONY
	mkdir -p $(OBJ_DIR)
	
$(EXE_DIR): .PHONY
	mkdir -p $(EXE_DIR)
	
#	Dependant libraries

../libmuHTTPd/libmuhttpd_release.a: .PHONY
	+@make -C ../libmuHTTPd
	
../libmuHTTPd/libmuhttpd_debug.a: .PHONY
	+@make DEBUG=1 -C ../libmuHTTPd

depends: .PHONY
	makedepend -Y -I ../libmuHTTPd/ -p'$$(OBJ_DIR)/' *.cpp

.PHONY:
	@true


# DO NOT DELETE

$(OBJ_DIR)/main.o: ../libmuHTTPd/dbgutils.h ../libmuHTTPd/serverClasses.h
$(OBJ_DIR)/main.o: ../libmuHTTPd/mustylesheet.h webpages.h
$(OBJ_DIR)/main.o: ../libmuHTTPd/muhttpd.h ../libmuHTTPd/mutable.h
$(OBJ_DIR)/webpages.o: ../libmuHTTPd/serverClasses.h
$(OBJ_DIR)/webpages.o: ../libmuHTTPd/mustylesheet.h ../libmuHTTPd/muhttpd.h
$(OBJ_DIR)/webpages.o: ../libmuHTTPd/mutable.h ../libmuHTTPd/muhtmlhelpers.h
$(OBJ_DIR)/webpages.o: ../libmuHTTPd/muutils.h ../libmuHTTPd/dbgutils.h
$(OBJ_DIR)/webpages.o: image/html_02_00_jpg.h image/html_debuggerfe_ico.h
