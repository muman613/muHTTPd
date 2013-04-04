################################################################################
#	MODULE		:	buildsys.mk
#	AUTHOR 		:	Michael A. Uman
#	DATE		:	April 3, 2013
################################################################################

CPP_OBJS=$(CPP_SOURCES:%.cpp=$(OBJ_DIR)/%.o)
OBJS=$(CPP_OBJS)

ifdef DEBUG
	EXE_DIR=bin/Debug
	OBJ_DIR=obj/Debug
	CFLAGS+= -D_DEBUG=1 -g3
else
	EXE_DIR=bin/Release
	OBJ_DIR=obj/Release
	CFLAGS+= -DNDEBUG -O2
endif

################################################################################
#	Generate the targets full name
################################################################################
ifeq ($(TARGET_TYPE), exe)
TARGET=$(EXE_DIR)/$(TARGET_EXE)
endif
ifeq ($(TARGET_TYPE), statlib)
TARGET=$(LIBNAME).a
endif

CFLAGS+=$(INCLUDES)
LDFLAGS+=$(EXTERN_LIBS) $(LIBS)


#	Default rule
$(OBJ_DIR)/%.o : %.cpp Makefile
	@echo "Compiling $*.cpp"
	@$(GCC) -c -o $(OBJ_DIR)/$*.o $(CFLAGS) $*.cpp

################################################################################
#	executable target
################################################################################
ifeq ($(TARGET_TYPE), exe)
$(TARGET): objdir exedir $(OBJS) $(EXTERN_LIBS)
	@echo "Linking $(TARGET)"
	@$(GCC) -o $(TARGET) $(OBJS) $(LDFLAGS)
endif

################################################################################
#	static library target
################################################################################
ifeq ($(TARGET_TYPE), statlib)
$(TARGET): objdir $(OBJS)
	@echo "Generating static library $(TARGET)"
	@$(AR) -r -s $(TARGET) $(OBJS)
endif

clean:
	rm -rf $(OBJS) $(TARGET)

objdir: .PHONY
	@echo "Creating object directory..."
	@mkdir -p $(OBJ_DIR) 

exedir: .PHONY	
	@echo "Creating exe directory..."
	@mkdir -p $(EXE_DIR)

depends:
	makedepend -Y  $(INCLUDES) -p'$$(OBJ_DIR)/' *.cpp

.PHONY:
	@true

