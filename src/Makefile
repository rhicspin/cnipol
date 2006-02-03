# File name           : makefile
#
# Author              : Osamu Jinouchi
# Creation data       : 2002/Dec

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

target : asymrhic Asym

OBJECTS = asym_main.o asym_process.o asym_calc.o hb_lib.o 

AsymOBJ = AsymMain.o AsymProcess.o AsymCalc.o hb_lib.o AsymHbook.o AsymRead.o AsymRunDB.o

######## BINARY ##################
asymrhic : $(OBJECTS)
	$(CC) $(C_FLAG) $(OBJECTS) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) -o $@

Asym : $(AsymOBJ)
	$(CC) $(C_FLAG) $(AsymOBJ) $(INC_PATHS) $(LIB_PATHS) $(LIBS) $(FLIB) -o $@


######## OBJECT ##################

asym_main.o : asymrhic.h
asym_process.o : asymrhic.h
asym_calc.o : asymrhic.h
AsymMain.o : asymrhic.h AsymMain.h

.C.o:
	$(CC) $(C_FLAG) $(D_FLAG) -c -I. $<
.f.o:
	$(FC) -O -c $<

install : asymrhic Asym
	install -c $^ $(HOME)/bin/.

##############################
clean :
	rm *.o




