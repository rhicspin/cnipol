CDEV=/usr/local/lib/cdev

LIB_PATHS = -L/usr/local/CDEV/store/X86/lib -L$(CDEV) -L$(CDEV)/Linux  
INC_PATHS = -I/usr/local/include/cdev -I../include
LIBS = -lcdev -lm -ldl -lstdc++ -lpthread

FFLAGS = -ffixed-line-length-none
CFLAGS = $(INC_PATHS) -pthread
CXXFLAGS = $(INC_PATHS) -pthread
LOADLIBES = $(LIB_PATHS) $(LIBS)
FC = f95

all : $(PROGS)

clean :
	-rm -f $(PROGS)
	-rm -f *.o

install:
	cp $(PROGS) $(IPROGS) /usr/local/polarim/bin
ifdef MPROGS
	cp $(MPROGS) /usr/local/polarim/bin/macro
endif
