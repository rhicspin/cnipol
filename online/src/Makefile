CDEV=/usr/local/lib/cdev

LIB_PATHS = -L$(CDEV) -L$(CDEV)/Linux -L/usr/local/cern/lib
INC_PATHS = -I/usr/local/include/cdev -I/usr/local/cern/include/cfortran
LIBS =  -lcdev -lm -ldl -lstdc++ -lpthread

CFLAGS = $(INC_PATHS) -pthread
CXXFLAGS = $(INC_PATHS) -pthread
LOADLIBES = $(LIB_PATHS) $(LIBS)

target : rhicpol rhic2hbook sync2rtdltime jet2root rpolado sndpic

rhicpol : rhicpol.o rpolutil.o rpolutilp.o stat2str.o ../libcmc/libcmc.o

rhic2hbook : rhic2hbook.o rdatautil.o rdatautilp.o stat2str.o
	g77 $^ -Wno-globals -o $@ $(LIB_PATHS) \
	 -lminuit -lpacklib -lkernlib -lmathlib \
	 -lcdev -lm -ldl -lstdc++

jet2root : jet2root.cpp
	g++ $(shell root-config --cflags) $^ -o $@ $(shell root-config --libs)

rpolado : rpolado.o stat2str.o

sync2rtdltime : sync2rtdltime.o

sndpic : sndpic.o

clean :
	-rm -f rhicpol
	-rm -f rhic2hbook
	-rm -f jet2root
	-rm -f rpolado
	-rm -f sndpic
	-rm -f sync2rtdltime
	-rm -f *.o
