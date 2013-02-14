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
	EXTERN_LIBS+=../libmyHTTPd/libmyhttpd_debug.a
else
	EXE_DIR=bin/Release
	WX_CFLAGS=$(shell $(WX_CONFIG) --cflags --unicode=no --debug=no)
	WX_LDFLAGS=$(shell $(WX_CONFIG) --libs --unicode=no --debug=no)
	OBJ_DIR=obj/Release
	EXTERN_LIBS+=../libmyHTTPd/libmyhttpd_release.a
endif

TARGET=$(EXE_DIR)/crcServantWeb

INCLUDES=-I ../libmyHTTPd/
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

../libmyHTTPd/libmyhttpd_release.a: .PHONY
	+@make -C ../libmyHTTPd
	
../libmyHTTPd/libmyhttpd_debug.a: .PHONY
	+@make DEBUG=1 -C ../libmyHTTPd

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

configfunctions.o: /usr/include/stdio.h /usr/include/features.h
configfunctions.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
configfunctions.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
configfunctions.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
configfunctions.o: /usr/include/bits/typesizes.h /usr/include/libio.h
configfunctions.o: /usr/include/_G_config.h /usr/include/wchar.h
configfunctions.o: /usr/include/bits/stdio_lim.h
configfunctions.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
configfunctions.o: /usr/include/sys/types.h /usr/include/time.h
configfunctions.o: /usr/include/endian.h /usr/include/bits/endian.h
configfunctions.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
configfunctions.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
configfunctions.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
configfunctions.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
configfunctions.o: /usr/include/string.h /usr/include/xlocale.h
configfunctions.o: crcServantContext.h configfunctions.h crcServant.h
crcServantContext.o: crcServantContext.h sysutils.h
crcServant.o: /usr/include/stdio.h /usr/include/features.h
crcServant.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
crcServant.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
crcServant.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
crcServant.o: /usr/include/bits/typesizes.h /usr/include/libio.h
crcServant.o: /usr/include/_G_config.h /usr/include/wchar.h
crcServant.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
crcServant.o: /usr/include/stdlib.h /usr/include/sys/types.h
crcServant.o: /usr/include/time.h /usr/include/endian.h
crcServant.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
crcServant.o: /usr/include/sys/select.h /usr/include/bits/select.h
crcServant.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
crcServant.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
crcServant.o: /usr/include/alloca.h /usr/include/popt.h crcServant.h
crcServant.o: /usr/include/dialog.h /usr/include/dlg_config.h
crcServant.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
crcServant.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
crcServant.o: /usr/include/bits/confname.h /usr/include/getopt.h
crcServant.o: /usr/include/ctype.h /usr/include/xlocale.h
crcServant.o: /usr/include/string.h /usr/include/signal.h
crcServant.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
crcServant.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
crcServant.o: /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h
crcServant.o: /usr/include/math.h /usr/include/bits/huge_val.h
crcServant.o: /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h
crcServant.o: /usr/include/bits/inf.h /usr/include/bits/nan.h
crcServant.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
crcServant.o: /usr/include/curses.h /usr/include/ncurses_dll.h
crcServant.o: /usr/include/unctrl.h /usr/include/libintl.h
crcServant.o: /usr/include/langinfo.h /usr/include/nl_types.h
crcServant.o: /usr/include/bits/locale.h reportgen.h crcServantWeb.h
crcServant.o: configfunctions.h myintmessagequeue.h sysutils.h
crcServant.o: crcServantContext.h
main.o: /usr/include/stdio.h /usr/include/features.h
main.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
main.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
main.o: /usr/include/bits/typesizes.h /usr/include/libio.h
main.o: /usr/include/_G_config.h /usr/include/wchar.h
main.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
main.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
main.o: /usr/include/endian.h /usr/include/bits/endian.h
main.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
main.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
main.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
main.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
main.o: /usr/include/string.h /usr/include/xlocale.h /usr/include/unistd.h
main.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
main.o: /usr/include/getopt.h /usr/include/sys/wait.h /usr/include/signal.h
main.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
main.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
main.o: /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h
main.o: /usr/include/sys/resource.h /usr/include/bits/resource.h
main.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
main.o: /usr/include/dialog.h /usr/include/dlg_config.h /usr/include/fcntl.h
main.o: /usr/include/bits/fcntl.h /usr/include/ctype.h /usr/include/math.h
main.o: /usr/include/bits/huge_val.h /usr/include/bits/huge_valf.h
main.o: /usr/include/bits/huge_vall.h /usr/include/bits/inf.h
main.o: /usr/include/bits/nan.h /usr/include/bits/mathdef.h
main.o: /usr/include/bits/mathcalls.h /usr/include/curses.h
main.o: /usr/include/ncurses_dll.h /usr/include/unctrl.h
main.o: /usr/include/libintl.h /usr/include/langinfo.h
main.o: /usr/include/nl_types.h /usr/include/bits/locale.h crcServantWeb.h
main.o: crcServant.h configfunctions.h reportgen.h myintmessagequeue.h
main.o: sysutils.h crcServantContext.h webpages.h
myintmessagequeue.o: myintmessagequeue.h
myunpackager.o: /usr/include/libtar.h /usr/include/sys/types.h
myunpackager.o: /usr/include/features.h /usr/include/bits/predefs.h
myunpackager.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
myunpackager.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
myunpackager.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
myunpackager.o: /usr/include/time.h /usr/include/endian.h
myunpackager.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
myunpackager.o: /usr/include/sys/select.h /usr/include/bits/select.h
myunpackager.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
myunpackager.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
myunpackager.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
myunpackager.o: /usr/include/tar.h /usr/include/libtar_listhash.h
myunpackager.o: /usr/include/errno.h /usr/include/bits/errno.h
myunpackager.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
myunpackager.o: /usr/include/asm-generic/errno.h
myunpackager.o: /usr/include/asm-generic/errno-base.h myunpackager.h
reportgen.o: /usr/include/stdio.h /usr/include/features.h
reportgen.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
reportgen.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
reportgen.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
reportgen.o: /usr/include/bits/typesizes.h /usr/include/libio.h
reportgen.o: /usr/include/_G_config.h /usr/include/wchar.h
reportgen.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
reportgen.o: /usr/include/stdlib.h /usr/include/sys/types.h
reportgen.o: /usr/include/time.h /usr/include/endian.h
reportgen.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
reportgen.o: /usr/include/sys/select.h /usr/include/bits/select.h
reportgen.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
reportgen.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
reportgen.o: /usr/include/alloca.h /usr/include/string.h
reportgen.o: /usr/include/xlocale.h /usr/include/dialog.h
reportgen.o: /usr/include/dlg_config.h /usr/include/fcntl.h
reportgen.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
reportgen.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
reportgen.o: /usr/include/getopt.h /usr/include/ctype.h /usr/include/signal.h
reportgen.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
reportgen.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
reportgen.o: /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h
reportgen.o: /usr/include/math.h /usr/include/bits/huge_val.h
reportgen.o: /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h
reportgen.o: /usr/include/bits/inf.h /usr/include/bits/nan.h
reportgen.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
reportgen.o: /usr/include/curses.h /usr/include/ncurses_dll.h
reportgen.o: /usr/include/unctrl.h /usr/include/libintl.h
reportgen.o: /usr/include/langinfo.h /usr/include/nl_types.h
reportgen.o: /usr/include/bits/locale.h reportgen.h crcServant.h
reportgen.o: crcServantContext.h
sysutils.o: /usr/include/unistd.h /usr/include/features.h
sysutils.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
sysutils.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
sysutils.o: /usr/include/gnu/stubs-64.h /usr/include/bits/posix_opt.h
sysutils.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
sysutils.o: /usr/include/bits/confname.h /usr/include/getopt.h
sysutils.o: /usr/include/errno.h /usr/include/bits/errno.h
sysutils.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
sysutils.o: /usr/include/asm-generic/errno.h
sysutils.o: /usr/include/asm-generic/errno-base.h /usr/include/sys/wait.h
sysutils.o: /usr/include/signal.h /usr/include/bits/sigset.h
sysutils.o: /usr/include/bits/signum.h /usr/include/time.h
sysutils.o: /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h
sysutils.o: /usr/include/bits/sigcontext.h /usr/include/bits/sigstack.h
sysutils.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h
sysutils.o: /usr/include/sys/resource.h /usr/include/bits/resource.h
sysutils.o: /usr/include/bits/time.h /usr/include/bits/waitflags.h
sysutils.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
sysutils.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
sysutils.o: crcServant.h crcServantWeb.h configfunctions.h reportgen.h
sysutils.o: myintmessagequeue.h sysutils.h crcServantContext.h
webpages.o: /usr/include/stdio.h /usr/include/features.h
webpages.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
webpages.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
webpages.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
webpages.o: /usr/include/bits/typesizes.h /usr/include/libio.h
webpages.o: /usr/include/_G_config.h /usr/include/wchar.h
webpages.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
webpages.o: /usr/include/stdlib.h /usr/include/sys/types.h
webpages.o: /usr/include/time.h /usr/include/endian.h
webpages.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
webpages.o: /usr/include/sys/select.h /usr/include/bits/select.h
webpages.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
webpages.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
webpages.o: /usr/include/alloca.h /usr/include/string.h
webpages.o: /usr/include/xlocale.h /usr/include/unistd.h
webpages.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
webpages.o: /usr/include/getopt.h /usr/include/sys/wait.h
webpages.o: /usr/include/signal.h /usr/include/bits/signum.h
webpages.o: /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h
webpages.o: /usr/include/bits/sigcontext.h /usr/include/bits/sigstack.h
webpages.o: /usr/include/bits/sigthread.h /usr/include/sys/resource.h
webpages.o: /usr/include/bits/resource.h /usr/include/bits/waitflags.h
webpages.o: /usr/include/bits/waitstatus.h crcServantWeb.h crcServant.h
webpages.o: configfunctions.h reportgen.h myintmessagequeue.h sysutils.h
webpages.o: crcServantContext.h webpages.h myunpackager.h image/favicon.h
webpages.o: image/clock_animated_gif.h image/texture_jpg.h
