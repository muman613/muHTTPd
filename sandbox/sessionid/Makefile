TARGET=sessionid
SOURCE_CPP=sessionid.cpp

OBJ_CPP=$(SOURCE_CPP:%.cpp=%.o)

OBJ=$(OBJ_CPP)

SQLITE3_LIBS=$(shell pkg-config --libs sqlite3)

CFLAGS=
LDFLAGS=
LIBS=$(SQLITE3_LIBS)

CPP=g++

%.o : %.cpp
	$(CPP) -c $(CFLAGS) -o $*.o $*.cpp
	
$(TARGET) : $(OBJ)
	$(CPP) -o $(TARGET) $(LDFLAGS) $(OBJ) $(LIBS)

clean: .PHONY
	rm -rf $(TARGET) $(OBJ)
	
.PHONY:
	@true
