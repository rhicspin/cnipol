
C++    = g++

SRC    = calcPhi.cc  interval.cc
BINARY = calcPhi interval

all: calcPhi interval

clean:
	rm -f *.o core 


calcPhi: calcPhi.o
	$(C++) $^ -o $@


interval: interval.o
	$(C++) $^ -o $@

install: $(BINARY)
	install -v -C $^ $(INSTALLDIR)



