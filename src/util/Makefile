
C++    = g++

SRC    = calcPhi.cc  interval.cc converge.cc
BINARY = calcPhi interval converge

all: calcPhi interval converge

clean:
	rm -f *.o core 


calcPhi: calcPhi.o
	$(C++) $^ -o $@


interval: interval.o
	$(C++) $^ -o $@

converge: converge.o
	$(C++) $^ -o $@

install: $(BINARY)
	install -v -C $^ $(INSTALLDIR)



