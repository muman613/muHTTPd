###############################################################################
#	Makefile
###############################################################################

CPP_SOURCES=sessionid.cpp
TARGET_EXE=sessionid

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

CPP_OBJS=$(CPP_SOURCES:%.cpp=$(OBJ_DIR)/%.o)
OBJS=$(CPP_OBJS)

#	Default rule
$(OBJ_DIR)/%.o : %.cpp Makefile
	@echo "Compiling $*.cpp"
	$(GCC) -c -o $(OBJ_DIR)/$*.o $(CFLAGS) $*.cpp

$(TARGET): $(OBJ_DIR) $(EXE_DIR) $(OBJS) $(EXTERN_LIBS)
	@echo "Linking $(TARGET)"
	$(GCC) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:	.PHONY
	rm -rf $(OBJS) $(TARGET)

$(OBJ_DIR) : .PHONY
	mkdir -p $(OBJ_DIR)

$(EXE_DIR): .PHONY
	mkdir -p $(EXE_DIR)

depends: .PHONY
	makedepend -Y $(INCLUDES) -p'$$(OBJ_DIR)/' *.cpp

.PHONY:
	@true

# DO NOT DELETE
