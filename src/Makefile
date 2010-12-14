# File name           : Makefile
#
# Author              : Itaru Nakagawa
#                     : Dmitri Smirnov
#
# Creation data       : Feb.6,2006

#LIB_PATHS = -L/usr/lib -L/usr/X11R6/lib -L/usr/local/lib -L/usr/local/cern/lib -L$(CNIDIR)/lib
LIB_PATHS = -L/usr/lib -L$(CNIDIR)/lib -L/usr/local/cern/lib
INC_PATHS = -I. -I/usr/include -I$(ROOTSYS)/include -I$(CNIDIR)/inc
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
FLIB = -lg2c -lgcc -lm -lnsl -lgfortran
FLIB1 =   # -lg2c

LIB_DIR    = $(CNIDIR)/lib
CNIANA_LIB = $(LIB_DIR)/libcniana.so
CNIPOL_LIB = $(LIB_DIR)/libcnipol.so

AsymOBJ = AsymMain.o AsymRoot.o AsymProcess.o AsymCalc.o hb_lib.o AsymHbook.o \
	  AsymRead.o AsymRunDB.o AsymRecover.o AsymErrorDetector.o Kinema.o \
          Asym.o

RunDBReaderOBJ = AsymRunDB.o RunDBReader.o Asym.o

dLayerCheckerOBJ = AsymRunDB.o dLayerChecker.o Asym.o

KinFuncOBJ = KinFunc.o Kinema.o Asym.o

AsymPlotOBJ = AsymPlot.o AsymRoot.o Asym.o AsymRunDB.o

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

all: $(BINARY)

lib: $(CNIPOL_LIB)
#	@echo "SRCS: $(SRCS)"
	@echo "CNIPOL_LIB_OBJS: $(CNIPOL_LIB_OBJS)"

$(CNIPOL_LIB): $(CNIPOL_LIB_OBJS)
	$(CC) $(SOFLAGS) $(CNIPOL_LIB_OBJS) $(ROOTLIBS) $(LIB_PATHS) $(LIBS) $(FLIB) -o $(CNIPOL_LIB)
	@echo "$@ done"

######## BINARY ##################

Asym: $(AsymOBJ) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(AsymOBJ) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIBS) -o $@

RunDBReader : $(RunDBReaderOBJ) $(CNIANA_LIB)
	$(CC) $(C_FLAG) $(RunDBReaderOBJ) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIB) -o $@

AsymPlot : $(AsymPlotOBJ)
	$(CC) $(C_FLAG) $(AsymPlotOBJ) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIB) -o $@

dLayerChecker : $(dLayerCheckerOBJ)
	$(CC) $(C_FLAG) $(dLayerCheckerOBJ) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIB) -o $@

KinFunc : $(KinFuncOBJ)
	$(CC) $(C_FLAG) $(KinFuncOBJ) $(INC_PATHS) $(ROOTLIB) -o $@

test : $(testOBJ) 
	$(CC) $(C_FLAG) $(testOBJ) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIB) -o $@

$(CNIANA_LIB): $(wildcard $(CNIDIR)/ana/*.cxx) $(wildcard $(CNIDIR)/inc/*.h)
	make -C $(CNIDIR)/ana

######## OBJECT ##################

AsymMain.o : Asym.h AsymMain.h AsymCalc.h WeightedMean.h rhicpol.h rpoldata.h 
AsymCalc.o : Asym.h AsymCalc.h AsymErrorDetector.h WeightedMean.h
AsymRecover.o : Asym.h AsymRecover.h AsymRecoverUserDefined.h
RunDBReader.o : AsymMain.h Asym.h
AsymRoot.o : AsymRoot.h
KinFunc.o  : AsymProcess.h
AsymProcess.o  : AsymProcess.h
#AsymPlot.o : AsymPlot.h AsymHeader.h
AsymPlot.o : AsymPlot.h

.f.o:
	$(FC) -O -c $<

Host:
	echo -e -n "$(HOSTNAME) \n" > .compiled_host

install : $(BINARY)
	install -v -C $^ $(INSTALLDIR)
	install AsymHeader.h *.C -v -C --mode 666 $(MACRODIR)

clean : 
	rm -f $(BINARY) $(CNIPOL_LIB)
	rm -f *.o
	rm -f *~ 

lib.clean : 
	rm -f $(CNIPOL_LIB)
