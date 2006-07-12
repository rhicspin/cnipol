# File name           : Makefile
#
# Author              : Itaru Nakagawa
# Creation data       : Feb.6,2006

LIB_PATHS = -L/usr/lib -L/usr/X11R6/lib -L/usr/local/lib -L/usr/local/cern/lib
INC_PATHS = -I. -I/usr/include -I$(ROOTSYS)/include
LIBS =  -lpacklib -lkernlib -lmathlib -lpawlib -lgraflib -lgrafX11 -lm -lc 
ROOTLIB =`root-config --cflags` `root-config --libs` -lMinuit -Wl,-rpath,$(ROOTSYS)

CC = g++
##C_FLAG = -g  -O2 -Wall -w 
##C_FLAG = -O2 -ggdb 
C_FLAG = -O3  

CXXFLAGS = $(INC_PATHS)
LOADLIBES = $(LIB_PATHS) $(LIBS)

D_FLAG = -Df2cFortran
FC = g77
FLIB = -lg2c -lgcc -lm -lnsl


AsymOBJ = AsymMain.o AsymProcess.o AsymCalc.o hb_lib.o AsymHbook.o AsymRead.o AsymRunDB.o AsymRecover.o AsymROOT.o

RunDBReaderOBJ = AsymRunDB.o RunDBReader.o 

dLayerCheckerOBJ = AsymRunDB.o dLayerChecker.o

testOBJ = test.o

BINARY = Asym RunDBReader dLayerChecker
all: Asym RunDBReader dLayerChecker Host test

######## BINARY ##################

Asym : $(AsymOBJ)
	$(CC) $(C_FLAG) $(AsymOBJ) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIB) -o $@

RunDBReader : $(RunDBReaderOBJ)
	$(CC) $(C_FLAG) $(RunDBReaderOBJ) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) -o $@

dLayerChecker : $(dLayerCheckerOBJ)
	$(CC) $(C_FLAG) $(dLayerCheckerOBJ) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIB) -o $@

test : $(testOBJ)
	$(CC) $(C_FLAG) $(testOBJ) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) $(ROOTLIB) -o $@

######## OBJECT ##################

AsymMain.o : Asym.h AsymMain.h AsymCalc.h WeightedMean.h rhicpol.h rpoldata.h 
AsymCalc.o : Asym.h AsymCalc.h WeightedMean.h
AsymRecover.o : Asym.h AsymRecover.h AsymRecoverUserDefined.h
RunDBReader.o : AsymMain.h Asym.h
AsymROOT.o : AsymROOT.h

.C.o:
	$(CC) $(C_FLAG) $(D_FLAG) -c -I. $<
.f.o:
	$(FC) -O -c $<

Host:
	echo -e -n "$(HOSTNAME) \n" > .compiled_host

install : Asym RunDBReader dLayerChecker
	install -v -C $^ $(INSTALLDIR)

##############################
clean : 
	rm -f $(BINARY)
	rm -f *.o
	rm -f *~ 




