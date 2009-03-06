CDEV=/usr/local/lib/cdev

LIB_PATHS = -L/usr/local/CDEV/store/X86/lib -L$(CDEV) -L$(CDEV)/Linux -L/usr/local/cern/lib 
INC_PATHS = -I/usr/local/include/cdev -I/usr/local/cern/include/cfortran
LIBS =  -lcdev -lm -ldl -lstdc++ -lpthread

CFLAGS = $(INC_PATHS) -pthread
CXXFLAGS = $(INC_PATHS) -pthread
LOADLIBES = $(LIB_PATHS) $(LIBS)

target : rhicpol rhic2hbook sync2rtdltime rpolado sndpic printpos getCdevValue testrunid setCdevString sendFailResults

rhicpol : rhicpol.o rpolutil.o rpolutilp.o stat2str.o ../libcmc/libcmc.o

rhic2hbook : rhic2hbook.o rdatautil.o rdatautilp.o stat2str.o
	g77 $^ -Wno-globals -o $@ $(LIB_PATHS) \
	 -lminuit -lpacklib -lkernlib -lmathlib \
	 -lcdev -lm -ldl -lstdc++

rpolado : rpolado.o stat2str.o

sync2rtdltime : sync2rtdltime.o

sndpic : sndpic.o

printpos : printpos.o

getCdevValue : getCdevValue.o

testrunid : testrunid

setCdevString : setCdevString.o

sendFailResults : sendFailResults.o

clean :
	-rm -f rhicpol
	-rm -f rhic2hbook
	-rm -f jet2root
	-rm -f rpolado
	-rm -f sndpic
	-rm -f sync2rtdltime
	-rm -f printpos
	-rm -f getCdevValue
	-rm -f setCdevString
	-rm -f sendFailResults
	-rm -f *.o

