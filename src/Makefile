# File name           : Makefile
#
# Author              : Itaru Nakagawa
# Creation data       : Feb.6,2006

LIB_PATHS = -L/usr/lib -L/usr/X11R6/lib -L/usr/local/lib -L/usr/local/cern/lib
INC_PATHS = -I. -I/usr/include
LIBS =  -lpacklib -lkernlib -lmathlib -lpawlib -lgraflib -lgrafX11 -lm -lc 

CC = g++
##C_FLAG = -g  -O2 -Wall -w 
##C_FLAG = -O2 -ggdb 
C_FLAG = -O3  

CXXFLAGS = $(INC_PATHS)
LOADLIBES = $(LIB_PATHS) $(LIBS)

D_FLAG = -Df2cFortran
FC = g77
FLIB = -lg2c -lgcc -lm -lnsl

target : Asym RunDBReader

AsymOBJ = AsymMain.o AsymProcess.o AsymCalc.o hb_lib.o AsymHbook.o AsymRead.o AsymRunDB.o 
	  
RunDBReaderOBJ = AsymRunDB.o RunDBReader.o 

######## BINARY ##################

Asym : $(AsymOBJ)
	$(CC) $(C_FLAG) $(AsymOBJ) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) -o $@

RunDBReader : $(RunDBReaderOBJ)
	$(CC) $(C_FLAG) $(RunDBReaderOBJ) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) -o $@

######## OBJECT ##################

AsymMain.o : Asym.h AsymMain.h
RunDBReader.o : AsymMain.h Asym.h

.C.o:
	$(CC) $(C_FLAG) $(D_FLAG) -c -I. $<
.f.o:
	$(FC) -O -c $<

install : Asym RunDBReader
	install -c $^ $(INSTALLDIR)/bin/.

##############################
clean :
	rm *.o




