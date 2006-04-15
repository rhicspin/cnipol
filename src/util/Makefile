
C++    = g++

SRC    = calcPhi.cc  interval.cc converge.cc
BINARY = calcPhi interval converge 

CDEV=/usr/local/lib/cdev
LIB_PATHS = -L$(CDEV) -L$(CDEV)/Linux 
INC_PATHS = -I/usr/local/include/cdev 
LIBS =  -lcdev -lm -ldl -lstdc++ -lpthread
CFLAGS = $(INC_PATHS) -pthread 
CXXFLAGS = $(INC_PATHS) -pthread
LOADLIBES = $(LIB_PATHS) $(LIBS)

all: calcPhi interval converge 


calcPhi: calcPhi.o
	$(C++) $^ -o $@


interval: interval.o
	$(C++) $^ -o $@

converge: converge.o
	$(C++) $^ -o $@


clean: $(BINARY)
	rm -f $^
	rm -f *.o core 


install: $(BINARY)
	install -v -C $^ $(INSTALLDIR)



