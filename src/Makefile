#
# Authors      : Itaru Nakagawa
#                Dmitri Smirnov
#
# Creation data: Feb.6,2006
#

CNIPOL_LIB_DIR  = $(CNIPOL_DIR)/lib
CERNLIB_LIB_DIR = $(CERNLIB_DIR)/lib

ROOT_INC_PATH   = $(shell root-config --incdir)

LIB_PATHS = -L$(CNIPOL_LIB_DIR) -L$(CERNLIB_LIB_DIR) -L$(MYSQL_LIB_DIR) -L$(MYSQLPP_LIB_DIR) -L$(UTILS_LIB_DIR)
INC_PATHS = -I. -I/usr/include -I$(ROOT_INC_PATH) -I$(CNIPOL_DIR)/inc \
   -I$(CNIPOL_DIR)/online/include -I$(MYSQL_INCLUDE_DIR) \
   -I$(MYSQLPP_INCLUDE_DIR) -I$(UTILS_INC_DIR)

LIBS      = -lpacklib -lkernlib -lmathlib -lpawlib -lgraflib -lgrafX11 -lm -lc -lcniana -lmysqlpp -lmysqlclient -lutils
ROOTLIB   = `root-config --cflags` `root-config --libs` -lMinuit -lXMLIO -Wl,-rpath,$(ROOTSYS)
ROOTLIBS  = `root-config --libs`

CC = g++
C_FLAG = -Wall -m32 #ds: -O2 -ggdb -w -g
SOFLAGS = -shared -m32

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

BINARIES = asym masym RunDBReader dLayerChecker KinFunc AsymPlot
BINARY_OBJS = $(BINARIES:=.o)

HDRS = $(filter-out LinkDef%, $(wildcard *.h))
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o) $(CNIANA_DICT_OBJ)

#CNIPOL_LIB_OBJS = $(filter-out AsymHbook.o $(BINARY_OBJS), $(OBJS)) Asym.o
CNIPOL_LIB_OBJS = $(filter-out $(BINARY_OBJS), $(OBJS)) hb_lib.o
#CNIPOL_LIB_OBJS = $(OBJS) hb_lib.o

all: lib install

lib: $(OBJS) $(CNIPOL_LIB)
#	echo $(OBJS)
#	@echo "SRCS: $(SRCS)"
#	@echo "CNIPOL_LIB_OBJS: $(CNIPOL_LIB_OBJS)"

$(CNIANA_DICT_SRC): AnaInfo.h AnaMeasResult.h AnaFillResult.h AnaGlobResult.h MeasInfo.h LinkDef.h
	@echo "Generating dictionary $(CNIANA_DICT_SRC)..."
	@rootcint -f $(CNIANA_DICT_SRC) -c -I. -I$(CNIPOL_DIR)/online/include -I$(CNIPOL_DIR)/inc $^

#@rootcint -f $(CNIANA_DICT_SRC) -c $(INC_PATHS) $^

$(CNIPOL_LIB): $(CNIPOL_LIB_OBJS)
	$(CC) $(SOFLAGS) $(CNIPOL_LIB_OBJS) $(ROOTLIBS) $(LIB_PATHS) $(LIBS) $(FLIB) -o $(CNIPOL_LIB)
	@echo "$@ done"

asym: $(CNIPOL_LIB) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIBS) asym.o -o $@

masym: $(CNIPOL_LIB) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIBS) masym.o -o $@

RunDBReader: $(CNIPOL_LIB) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIB) RunDBReader.o -o $@

AsymPlot: $(CNIPOL_LIB)
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(LIBS) $(ROOTLIB) AsymPlot.o -o $@

dLayerChecker: $(CNIPOL_LIB)
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(LIBS) $(ROOTLIB) dLayerChecker.o -o $@

KinFunc: $(CNIPOL_LIB) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(LIBS) $(ROOTLIB) KinFunc.o -o $@

$(CNIANA_LIB): $(wildcard $(CNIPOL_DIR)/ana/*.cxx) $(wildcard $(CNIPOL_DIR)/inc/*.h)
	make -C $(CNIPOL_DIR)/ana

.f.o:
	$(FC) -m32 -O -c $<

%.o: %.cc %.h Asym.h AsymHeader.h
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
	cd $(CNIPOL_DIR)/bin/; rm -f $(BINARIES)

lib.clean: 
	rm -f $(CNIPOL_LIB)

cleanall: clean bin.clean lib.clean
	rm -f *~ 
