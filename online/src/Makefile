CDEV      = /usr/local/lib/cdev

LIB_PATHS = -L/usr/local/CDEV/store/X86/lib -L$(CDEV) -L$(CDEV)/Linux -L/usr/local/cern/lib
INC_PATHS = -I/usr/local/include/cdev -I/usr/local/cern/include/cfortran
LIBS      = -lcdev -lm -ldl -lstdc++ -lpthread
CXX       = g++
FC        = f95

FFLAGS    = -ffixed-line-length-none
CFLAGS    = $(INC_PATHS) -pthread
CXXFLAGS  = $(INC_PATHS) -pthread
LOADLIBES = $(LIB_PATHS) $(LIBS)

target : rhicpol rhic2hbook sync2rtdltime rpolado sndpic printpos getCdevValue setCdevString sendFailResults

rhicpol : globals.o rpolutilp.o rhicpol.o rpolutil.o stat2str.o ../libcmc/libcmc.o

rhic2hbook : rhic2hbook.o rdatautil.o rdatautilp.o stat2str.o
	$(FC) $^ -Wno-globals -o $@ $(LIB_PATHS) \
	 -lpacklib -lkernlib -lmathlib \
	 -lcdev -lm -ldl -lstdc++

rpolado : rpolado.o stat2str.o globals.o

sync2rtdltime : sync2rtdltime.o

sndpic : sndpic.o

printpos : printpos.o

getCdevValue : getCdevValue.o

setCdevString : setCdevString.o

sendFailResults : sendFailResults.o

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

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

