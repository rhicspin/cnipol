#
# Authors             : Itaru Nakagawa
#                       Dmitri Smirnov
#
# Creation data       : Feb.6,2006
#

CNIPOL_LIB_DIR  = $(CNIPOL_DIR)/lib
CERNLIB_LIB_DIR = $(CERNLIB_DIR)/lib

LIB_PATHS = -L$(CNIPOL_LIB_DIR) -L$(CERNLIB_LIB_DIR)
INC_PATHS = -I. -I/usr/include -I`root-config --incdir` -I$(CNIPOL_DIR)/inc
LIBS      = -lpacklib -lkernlib -lmathlib -lpawlib -lgraflib -lgrafX11 -lm -lc -lcniana
ROOTLIB   = `root-config --cflags` `root-config --libs` -lMinuit -lXMLIO -Wl,-rpath,$(ROOTSYS)
ROOTLIBS  = `root-config --libs`

CC = g++
##C_FLAG = -g  -O2 -Wall -w 
##C_FLAG = -O2 -ggdb 
C_FLAG = -Wall #ds: -O3  
SOFLAGS = -shared

CXXFLAGS  = $(C_FLAG) $(INC_PATHS)
LOADLIBES = $(LIB_PATHS) $(LIBS)

#D_FLAG = -Df2cFortran
FC = g77
FLIB = -lm -lnsl -lgfortran

CNIANA_LIB = $(CNIPOL_LIB_DIR)/libcniana.so
CNIPOL_LIB = $(CNIPOL_LIB_DIR)/libcnipol.so

AsymOBJ = AsymMain.o AsymRoot.o AsymProcess.o AsymCalc.o hb_lib.o AsymHbook.o \
          AsymRead.o AsymRunDB.o AsymRecover.o AsymErrorDetector.o Kinema.o \
          Asym.o RunInfo.o AnaInfo.o AnaResult.o TargetInfo.o 

RunDBReaderOBJ = RunDBReader.o Asym.o AsymRunDB.o AsymRoot.o RunInfo.o AnaInfo.o AnaResult.o TargetInfo.o

dLayerCheckerOBJ = dLayerChecker.o Asym.o AsymRunDB.o AsymRoot.o RunInfo.o AnaInfo.o AnaResult.o TargetInfo.o

KinFuncOBJ = KinFunc.o Kinema.o Asym.o AsymRunDB.o AsymRoot.o RunInfo.o AnaInfo.o AnaResult.o TargetInfo.o 

AsymPlotOBJ = AsymPlot.o Asym.o AsymRunDB.o AsymRoot.o RunInfo.o AnaInfo.o AnaResult.o TargetInfo.o 

testOBJ = test.o AsymRoot.o

BINARY = Asym RunDBReader dLayerChecker KinFunc AsymPlot
BINARY_OBJS = $(BINARY:=.o)

HDRS = $(wildcard *.h)
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o)

#CNIPOL_LIB_OBJS = $(filter-out AsymHbook.o $(BINARY_OBJS), $(OBJS)) Asym.o hb_lib.o
CNIPOL_LIB_OBJS = $(filter-out $(BINARY_OBJS), $(OBJS)) Asym.o hb_lib.o
#CNIPOL_LIB_OBJS = Asym.o
#CNIPOL_LIB_OBJS = $(AsymOBJ) 

all: lib $(BINARY) install

lib: $(CNIPOL_LIB)
#	@echo "SRCS: $(SRCS)"
#	@echo "CNIPOL_LIB_OBJS: $(CNIPOL_LIB_OBJS)"

$(CNIPOL_LIB): $(CNIPOL_LIB_OBJS)
	$(CC) $(SOFLAGS) $(CNIPOL_LIB_OBJS) $(ROOTLIBS) $(LIB_PATHS) $(LIBS) $(FLIB) -o $(CNIPOL_LIB)
	@echo "$@ done"

######## BINARY ##################

Asym: $(AsymOBJ) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(AsymOBJ) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIBS) -o $@

RunDBReader : $(RunDBReaderOBJ) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(RunDBReaderOBJ) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIB) -o $@

AsymPlot : $(AsymPlotOBJ)
	$(CC) $(C_FLAG) $(AsymPlotOBJ) $(LIB_PATHS) $(LIBS) $(ROOTLIB) -o $@

dLayerChecker : $(dLayerCheckerOBJ)
	$(CC) $(C_FLAG) $(dLayerCheckerOBJ) $(LIB_PATHS) $(LIBS) $(ROOTLIB) -o $@

KinFunc : $(KinFuncOBJ) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(KinFuncOBJ) $(LIB_PATHS) $(LIBS) $(ROOTLIB) -o $@

test : $(testOBJ) 
	$(CC) $(C_FLAG) $(testOBJ) $(LIB_PATHS) $(LIBS) $(ROOTLIB) -o $@

$(CNIANA_LIB): $(wildcard $(CNIPOL_DIR)/ana/*.cxx) $(wildcard $(CNIPOL_DIR)/inc/*.h)
	make -C $(CNIPOL_DIR)/ana

######## OBJECT ##################

AsymMain.o:    Asym.h AsymMain.h AsymCalc.h rhicpol.h rpoldata.h 
AsymCalc.o:    Asym.h AsymCalc.h AsymErrorDetector.h
AsymRecover.o: Asym.h AsymRecover.h AsymRecoverUserDefined.h
RunDBReader.o: AsymRunDB.h
AsymRoot.o:    AsymRoot.h
KinFunc.o:     Kinema.h
AsymProcess.o: AsymProcess.h
#AsymPlot.o:    AsymPlot.h AsymHeader.h
AsymPlot.o:    AsymPlot.h
RunInfo.o:     RunInfo.h
AnaInfo.o:     AnaInfo.h
TargetInfo.o:  TargetInfo.h
AnaResult.o:   AnaResult.h
Asym.o:        Asym.h RunInfo.h AnaInfo.h TargetInfo.h AnaResult.h AsymRunDB.h

.f.o:
	$(FC) -O -c $<

Host:
	echo -e -n "$(HOSTNAME) \n" > .compiled_host

install : $(BINARY)
	install -v -C $^ $(CNIPOL_DIR)/bin
	#install AsymHeader.h *.C -v -C --mode 666 $(MACRODIR)

clean: 
	rm -f $(BINARY) $(CNIPOL_LIB)
	rm -f *.o
	rm -f *~ 

bin.clean: lib.clean
	cd $(CNIPOL_DIR)/bin/; rm -f $(BINARY)

lib.clean: 
	rm -f $(CNIPOL_LIB)
