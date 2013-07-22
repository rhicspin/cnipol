#
# Authors :
#
#    Dmitri Smirnov
#
# Creation data: Feb 6, 2006
#

CNIPOL_LIB_DIR  = $(CNIPOL_DIR)/lib
CERNLIB_LIB_DIR = $(CERNLIB_DIR)/lib

ROOT_INC_PATH   = $(shell root-config --incdir)

LIB_PATHS = -L$(CNIPOL_LIB_DIR) -L$(CERNLIB_LIB_DIR) -L$(MYSQL_LIB_DIR) -L$(MYSQLPP_LIB_DIR) -L$(UTILS_LIB_DIR)
INC_PATHS = -I. -I/usr/include -I$(ROOT_INC_PATH) -I$(CNIPOL_DIR)/inc \
   -I$(CNIPOL_DIR)/online/include -I$(MYSQL_INCLUDE_DIR) \
   -I$(MYSQLPP_INCLUDE_DIR) -I$(UTILS_INC_DIR)

#LIBS         = -lpacklib -lkernlib -lmathlib -lpawlib -lgraflib -lgrafX11 -lm -lc -lcniana -lmysqlpp -lmysqlclient -lutils
LIBS         = -lm -lc -lcniana -lmysqlpp -lmysqlclient -lutils
ROOTLIB      = `root-config --cflags` `root-config --libs` -lMinuit -lXMLIO -Wl,-rpath,$(ROOTSYS)
ROOTLIBS     = `root-config --libs`
ROOTLDFLAGS := $(shell root-config --nonew --ldflags)

SVN_ASYM_VERSION := $(shell svnversion -n $(CNIPOL_DIR))
#SVN_ASYM_VERSION = 'ASYM_VERSION="555"'

CC = g++
#C_FLAG = -Wall -m32 -D'ASYM_VERSION="$(SVN_ASYM_VERSION)"' -O2 -w
#C_FLAG = -Wall -m32 -D'ASYM_VERSION="$(SVN_ASYM_VERSION)"' -ggdb -g -fno-inline
C_FLAG = -Wall -fPIC $(ROOTLDFLAGS) -D'ASYM_VERSION="$(SVN_ASYM_VERSION)"' -ggdb -g -fno-inline
#SOFLAGS = -shared -m32
SOFLAGS = -shared $(ROOTLDFLAGS)

CXXFLAGS  = $(C_FLAG) $(INC_PATHS)
LOADLIBES = $(LIB_PATHS) $(LIBS)

#D_FLAG = -Df2cFortran
FC = g77
FLIB = -lm -lnsl -lgfortran

CNIANA_LIB = $(CNIPOL_LIB_DIR)/libcniana.so
CNIPOL_LIB = $(CNIPOL_LIB_DIR)/libcnipol.so

CNIANA_DICT_SRC = cnipol_dict.cxx
CNIANA_DICT_HDR = $(CNIANA_DICT_SRC:.cxx=.h)
CNIANA_DICT_OBJ = $(CNIANA_DICT_SRC:.cxx=.o)
CNIANA_DICT     = $(CNIANA_DICT_SRC) $(CNIANA_DICT_HDR) $(CNIANA_DICT_OBJ)

BINARIES = asym masym malpha RunDBReader dLayerChecker KinFunc AsymPlot plotAN spinflipper
BINARY_OBJS = $(BINARIES:=.o)

#BINARIES2 = plotAN 
#BINARY_OBJS2 = $(BINARIES2:=.o)

HDRS = $(filter-out LinkDef%, $(wildcard *.h))
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o) $(CNIANA_DICT_OBJ)

#CNIPOL_LIB_OBJS = $(filter-out $(BINARY_OBJS), $(OBJS)) hb_lib.o
CNIPOL_LIB_OBJS = $(filter-out $(BINARY_OBJS), $(OBJS))

.PHONY: all lib bin install

all: lib install

lib: $(BINARY_OBJS) $(CNIPOL_LIB)

bin: $(BINARIES)

$(CNIANA_DICT_SRC): AnaMeasResult.h AnaFillResult.h AnaGlobResult.h MeasInfo.h LinkDef.h
	@echo "Generating dictionary $(CNIANA_DICT_SRC)..."
	@echo rootcint -f $(CNIANA_DICT_SRC) -c -I. -I$(UTILS_INC_DIR) -I$(CNIPOL_DIR)/online/include -I$(CNIPOL_DIR)/inc $^
	@rootcint -f $(CNIANA_DICT_SRC) -c -I. -I$(UTILS_INC_DIR) -I$(CNIPOL_DIR)/online/include -I$(CNIPOL_DIR)/inc $^

#@rootcint -f $(CNIANA_DICT_SRC) -c $(INC_PATHS) $^

$(CNIPOL_LIB): $(CNIPOL_LIB_OBJS)
	$(CC) $(SOFLAGS) $(CNIPOL_LIB_OBJS) $(ROOTLIBS) $(LIB_PATHS) $(LIBS) $(FLIB) -o $(CNIPOL_LIB)
	@echo "$@ done"

.SECONDEXPANSION:
$(BINARIES): $$@.o
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(FLIB) $(LIBS) $(ROOTLIBS) $@.o -o $@
	@echo 
	@echo "$(SVN_ASYM_VERSION)"
	@echo 

$(CNIANA_LIB): $(wildcard $(CNIPOL_DIR)/ana/*.cxx) $(wildcard $(CNIPOL_DIR)/inc/*.h)
	make -C $(CNIPOL_DIR)/ana

.f.o:
	$(FC) -fPIC $(ROOTLDFLAGS) -O -c $<

%.o: %.cc %.h AsymCommon.h AsymHeader.h
	#echo $* $@
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(CNIANA_DICT_OBJ) : $(CNIANA_DICT_SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: $(BINARIES)
	install -v -p $^ $(CNIPOL_DIR)/bin
	rm -f $^

#install AsymHeader.h *.C -v -C --mode 666 $(MACRODIR)

clean: lib.clean
	rm -f $(BINARIES)
	rm -f *.o
	rm -f $(CNIANA_DICT)

bin.clean: lib.clean
	rm -f $(BINARIES)
	cd $(CNIPOL_DIR)/bin/; rm -f $(BINARIES)

lib.clean: 
	rm -f $(CNIPOL_LIB)

cleanall: clean bin.clean lib.clean
	rm -f *~ 
