CDEV=/usr/local/lib/cdev

LIB_PATHS = -L$(CDEV) -L$(CDEV)/Linux 
INC_PATHS = -I/usr/local/include/cdev 
LIBS =  -lcdev -lm -ldl -lstdc++ -lpthread

CFLAGS = $(INC_PATHS) -pthread 
CXXFLAGS = $(INC_PATHS) -pthread
LOADLIBES = $(LIB_PATHS) $(LIBS)

getPattern:	getPattern.o cdevPatUtil.o

install: getPattern
	install  -v -C $^ $(INSTALLDIR)

clean :
	-rm -f getPattern
	-rm -f *.o
	-rm -f *~
