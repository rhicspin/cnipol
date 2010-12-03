CDEV=/usr/local/lib/cdev

LIB_PATHS = -L/usr/local/CDEV/store/X86/lib -L$(CDEV) -L$(CDEV)/Linux  
INC_PATHS = -I/usr/local/include/cdev -I../include
LIBS =  -lcdev -lm -ldl -lstdc++ -lpthread

CFLAGS = $(INC_PATHS) -pthread
CXXFLAGS = $(INC_PATHS) -pthread
LOADLIBES = $(LIB_PATHS) $(LIBS)

PROGS = cdevCmd sndpic

all : $(PROGS)

clean :
	-rm -f $(PROGS)
	-rm -f *.o

