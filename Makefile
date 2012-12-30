#
#	Simple makefile
#

TARGET=myHTTPd
SRC=main.cpp  myhttpd.cpp  serverClasses.cpp  dbgutils.cpp mystylesheet.cpp \
    mytable.cpp webpages.cpp myutils.cpp

OBJ=$(SRC:%.cpp=%.o)
WX_CONFIG=wx-config

WX_CFLAGS=$(shell $(WX_CONFIG) --cflags --debug=yes)
WX_LDFLAGS=$(shell $(WX_CONFIG) --libs --debug=yes)

CFLAGS=-g3 -D_DEBUG=1 $(WX_CFLAGS)
CPP=g++
LDFLAGS=$(WX_LDFLAGS)

%.o : %.cpp
	$(CPP) -c $(CFLAGS) -o $*.o $*.cpp
	
$(TARGET) : $(OBJ)
	$(CPP) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:	
	rm -rf $(OBJ) $(TARGET)	
	
