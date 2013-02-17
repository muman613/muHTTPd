###############################################################################
#	Makefile
###############################################################################

CPP_SOURCES=main.cpp webpages.cpp

#	Tools

WX_CONFIG=/opt/wxWidgets/2.8.12/bin/wx-config
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

../DeviceUnderTest/libDeviceUnderTest.a: .PHONY
	+@make -C ../DeviceUnderTest

../DeviceUnderTest/libDeviceUnderTestd.a: .PHONY
	+@make DEBUG=1 -C ../DeviceUnderTest   

../libmuHTTPd/libmuhttpd_release.a: .PHONY
	+@make -C ../libmuHTTPd
	
../libmuHTTPd/libmuhttpd_debug.a: .PHONY
	+@make DEBUG=1 -C ../libmuHTTPd

../CTB/lib/Linux/libCTB.a: .PHONY
	+@make -C ../CTB
	
../CTB/lib/Linux/libCTBd.a: .PHONY
	+@make DEBUG=1 -C ../CTB

../libvector/libvectord.a: .PHONY
	+@make DEBUG=1 -C ../libvector

../libvector/libvector.a: .PHONY
	+@make -C ../libvector
    
    
.PHONY:
	@true


# DO NOT DELETE

$(OBJ_DIR)/main.o: /usr/include/stdio.h /usr/include/features.h
$(OBJ_DIR)/main.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
$(OBJ_DIR)/main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
$(OBJ_DIR)/main.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
$(OBJ_DIR)/main.o: /usr/include/bits/typesizes.h /usr/include/libio.h
$(OBJ_DIR)/main.o: /usr/include/_G_config.h /usr/include/wchar.h
$(OBJ_DIR)/main.o: /usr/include/bits/wchar.h /usr/include/xlocale.h
$(OBJ_DIR)/main.o: /usr/include/bits/stdio_lim.h
$(OBJ_DIR)/main.o: /usr/include/bits/sys_errlist.h /usr/include/signal.h
$(OBJ_DIR)/main.o: /usr/include/bits/sigset.h /usr/include/bits/signum.h
$(OBJ_DIR)/main.o: /usr/include/time.h /usr/include/bits/siginfo.h
$(OBJ_DIR)/main.o: /usr/include/bits/sigaction.h
$(OBJ_DIR)/main.o: /usr/include/bits/sigcontext.h
$(OBJ_DIR)/main.o: /usr/include/bits/sigstack.h /usr/include/sys/ucontext.h
$(OBJ_DIR)/main.o: /usr/include/bits/pthreadtypes.h
$(OBJ_DIR)/main.o: /usr/include/bits/sigthread.h ../libmuHTTPd/dbgutils.h
$(OBJ_DIR)/main.o: ../libmuHTTPd/serverClasses.h ../libmuHTTPd/mustylesheet.h
$(OBJ_DIR)/main.o: webpages.h
$(OBJ_DIR)/webpages.o: ../libmuHTTPd/serverClasses.h
$(OBJ_DIR)/webpages.o: ../libmuHTTPd/mustylesheet.h ../libmuHTTPd/dbgutils.h
$(OBJ_DIR)/webpages.o: image/html_02_00_jpg.h image/html_debuggerfe_ico.h
