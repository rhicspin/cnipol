
C++    = g++

SRC    = calcPhi.cc  interval.cc converge.cc CompareRunNmbr.cc mkTable.cc
BINARY = calcPhi interval converge CompareRunNmbr mkTable

CDEV=/usr/local/lib/cdev
LIB_PATHS = -L$(CDEV) -L$(CDEV)/Linux 
INC_PATHS = -I/usr/local/include/cdev 
LIBS =  -lcdev -lm -ldl -lstdc++ -lpthread
CFLAGS = $(INC_PATHS) -pthread 
CXXFLAGS = $(INC_PATHS) -pthread
LOADLIBES = $(LIB_PATHS) $(LIBS)

all: calcPhi interval converge CompareRunNmbr mkTable


calcPhi: calcPhi.o
	$(C++) $^ -o $@

CompareRunNmbr: CompareRunNmbr.o
	$(C++) $^ -o $@


interval: interval.o
	$(C++) $^ -o $@

converge: converge.o
	$(C++) $^ -o $@

mkTable: mkTable.o
	$(C++) $^ -o $@


clean: 
	rm -f $(BINARY)
	rm -f *.o core 
	rm -f *~

install: $(BINARY)
	install -v -C $^ $(INSTALLDIR)



