#
# Authors      : Itaru Nakagawa
#                Dmitri Smirnov
#
# Creation data: Feb.6,2006
#

CNIPOL_LIB_DIR  = $(CNIPOL_DIR)/lib
CERNLIB_LIB_DIR = $(CERNLIB_DIR)/lib

LIB_PATHS = -L$(CNIPOL_LIB_DIR) -L$(CERNLIB_LIB_DIR)
INC_PATHS = -I. -I/usr/include -I`root-config --incdir` -I$(CNIPOL_DIR)/inc -I$(CNIPOL_DIR)/online/include
LIBS      = -lpacklib -lkernlib -lmathlib -lpawlib -lgraflib -lgrafX11 -lm -lc -lcniana
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

BINARY = Asym RunDBReader dLayerChecker KinFunc AsymPlot
BINARY_OBJS = $(BINARY:=.o)

HDRS = $(wildcard *.h)
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o)

#CNIPOL_LIB_OBJS = $(filter-out AsymHbook.o $(BINARY_OBJS), $(OBJS)) Asym.o
CNIPOL_LIB_OBJS = $(filter-out $(BINARY_OBJS), $(OBJS)) Asym.o hb_lib.o

all: lib $(BINARY) install

lib: $(CNIPOL_LIB)
#	@echo "SRCS: $(SRCS)"
#	@echo "CNIPOL_LIB_OBJS: $(CNIPOL_LIB_OBJS)"

$(CNIPOL_LIB): $(CNIPOL_LIB_OBJS)
	$(CC) $(SOFLAGS) $(CNIPOL_LIB_OBJS) $(ROOTLIBS) $(LIB_PATHS) $(LIBS) $(FLIB) -o $(CNIPOL_LIB)
	@echo "$@ done"


Asym: $(CNIPOL_LIB) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIBS) -o $@

RunDBReader: $(CNIPOL_LIB) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIB) -o $@

AsymPlot: $(CNIPOL_LIB)
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(LIBS) $(ROOTLIB) -o $@

dLayerChecker: $(CNIPOL_LIB)
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(LIBS) $(ROOTLIB) -o $@

KinFunc: $(CNIPOL_LIB) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(CNIPOL_LIB) $(LIB_PATHS) $(LIBS) $(ROOTLIB) -o $@

$(CNIANA_LIB): $(wildcard $(CNIPOL_DIR)/ana/*.cxx) $(wildcard $(CNIPOL_DIR)/inc/*.h)
	make -C $(CNIPOL_DIR)/ana

AsymMain.o:    Asym.h AsymMain.h AsymCalc.h
AsymCalc.o:    Asym.h AsymCalc.h AsymErrorDetector.h
AsymRecover.o: Asym.h AsymRecover.h AsymRecoverUserDefined.h
RunDBReader.o: AsymRunDB.h
AsymRoot.o:    AsymRoot.h
KinFunc.o:     Kinema.h
AsymProcess.o: AsymProcess.h
AsymPlot.o:    AsymPlot.h
RunInfo.o:     RunInfo.h
AnaInfo.o:     AnaInfo.h
TargetInfo.o:  TargetInfo.h
AnaResult.o:   AnaResult.h
Asym.o:        Asym.h RunInfo.h AnaInfo.h TargetInfo.h AnaResult.h AsymRunDB.h DbEntry.h

.f.o:
	$(FC) -m32 -O -c $<

install : $(BINARY)
	install -v -p $^ $(CNIPOL_DIR)/bin
	rm -f $^

#install AsymHeader.h *.C -v -C --mode 666 $(MACRODIR)

clean: lib.clean
	rm -f $(BINARY)
	rm -f *.o
	rm -f *~ 

bin.clean: lib.clean
	cd $(CNIPOL_DIR)/bin/; rm -f $(BINARY)

lib.clean: 
	rm -f $(CNIPOL_LIB)

cleanall: clean bin.clean lib.clean
