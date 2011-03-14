#
# Makefile for CNIANA
#

LIB_DIR       = $(CNIPOL_DIR)/lib
SRC_DIR       = $(CNIPOL_DIR)/ana
INC_DIR       = $(CNIPOL_DIR)/inc
BIN_DIR       = $(CNIPOL_DIR)/bin

ARCH         := $(shell root-config --arch)

ObjSuf        = o
SrcSuf        = cxx
ExeSuf        =
DllSuf        = so

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

# Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
CXX           = g++
CC            = gcc

# if you plan to do a whole lot of debugging, remove '-O' switch(es)
#CXXFLAGS      = -g -O -Wall -fPIC -Wno-deprecated -I. -I$(INC_DIR) -I$(CNIPOL_DIR)/src
CXXFLAGS      = -g -O -Wall -fPIC -I. -I$(INC_DIR) -I$(CNIPOL_DIR)/src -I$(CNIPOL_DIR)/online/include
CFLAGS        = -g -O -I$(INC_DIR)
LD            = g++
LDFLAGS       = -g
SOFLAGS       = -shared -m32

CXXFLAGS     += $(ROOTCFLAGS)

LIBS          = $(ROOTLIBS) -lMinuit -lTreePlayer

LDFLAGS      += $(shell root-config --nonew --ldflags)

#------------------------------------------------------------------------------

CNIANA_LIB      = $(LIB_DIR)/libcniana.so

CNIANA_DICT_SRC = $(SRC_DIR)/cniana_dict.cxx
CNIANA_DICT_HDR = $(CNIANA_DICT_SRC:.cxx=.h)
CNIANA_DICT_OBJ = $(CNIANA_DICT_SRC:.cxx=.o)
CNIANA_DICT     = $(CNIANA_DICT_SRC) $(CNIANA_DICT_HDR) \
                  $(CNIANA_DICT_OBJ)

HDRS       = $(filter-out $(INC_DIR)/LinkDef%, $(wildcard $(INC_DIR)/*.h) )
SRCS_CXX   = $(filter-out $(CNIANA_DICT_SRC), $(wildcard $(SRC_DIR)/*.cxx)) \
             $(CNIANA_DICT_SRC)
OBJS       = $(SRCS_CXX:.cxx=.o)
CNIPOL_OBJ = $(CNIPOL_DIR)/src/Kinema.o

#------------------------------------------------------------------------------

#all: lib main-train snns2root
all: lib

lib: $(CNIANA_LIB)

$(CNIANA_LIB): $(OBJS) $(CNIPOL_OBJ)
	$(LD) $(SOFLAGS) $(OBJS) $(CNIPOL_OBJ) $(LIBS) -o $(CNIANA_LIB)
	@echo "$@ done"

snns2root: $(SRC_DIR)/snns2root.o
	$(LD) $(LDFLAGS) $(SRC_DIR)/snns2root.o \
	   -o $(BIN_DIR)/$@$(ExeSuf)
	@echo "$@ done"

$(CNIANA_DICT_SRC): $(HDRS) $(INC_DIR)/LinkDef.h
	@echo "Generating dictionary $(CNIANA_DICT_SRC)..."
	@rootcint -f $(CNIANA_DICT_SRC) -c -I$(CNIPOL_DIR)/src -I$(CNIPOL_DIR)/online/include $^

main-%: main-%.$(ObjSuf) $(OBJS)
	$(LD) $(LDFLAGS) $< $(OBJS) $(LIBS) -o $(BIN_DIR)/$*$(ExeSuf)
	@echo "$@ done"
	-cp $(SNNS_DIR)/tools/sources/ff_bignet $(BIN_DIR)/

main-%.o : main-%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(CNIANA_DICT_OBJ) : $(CNIANA_DICT_SRC)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(SRC_DIR)/%.o : $(SRC_DIR)/%.cxx $(INC_DIR)/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

#$(SRC_DIR)/%.o : $(SRC_DIR)/%.c
#	$(CC) $(CFLAGS) -c $< -o $@

#%.o : %.C %.h
#	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	@echo "removing binaries and supplementary files..."
	rm -f $(OBJS)
	rm -f $(CNIANA_LIB)
	rm -f $(CNIANA_DICT)

cleanall: clean
	@echo "removing executables in bin/..."
	rm -fr $(BIN_DIR)/*

echoenv:
	@echo ""
#
ifndef ROOTSYS
	@echo "ROOTSYS is not defined"
else
	@echo "ROOTSYS is set to $(ROOTSYS)"
endif
#
ifndef SNNS_DIR
	@echo "SNNS_DIR is not defined"
else
	@echo "SNNS_DIR is set to $(SNNS_DIR)"
endif

echo-%:
	@echo "$* = $($*)"
