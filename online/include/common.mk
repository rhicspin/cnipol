
#CDEV=/usr/local/lib/cdev
CDEV=/usr/local/cdev

#LIB_PATHS = -L/usr/local/CDEV/store/X86/lib -L$(CDEV) -L$(CDEV)/Linux  
LIB_PATHS = -L$(CDEV)/store/X86/lib
#INC_PATHS = -I/usr/local/include/cdev -I../include
INC_PATHS = -I$(CDEV)/store/X86/include/cdev -I../include
LIBS = -lcdev -lm -ldl -lstdc++ -lpthread

FFLAGS = -ffixed-line-length-none
CFLAGS = $(INC_PATHS) -pthread -Wall
CXXFLAGS = $(INC_PATHS) -pthread -Wall
LOADLIBES = $(LIB_PATHS) $(LIBS)
FC = f95
CXX = g++

all : $(PROGS)

clean :
	-rm -f $(PROGS)
	-rm -f *.o

install:
	#cp $(PROGS) $(IPROGS) /usr/local/polarim/bin
	cp $(PROGS) $(IPROGS) /usr/local/cnipol/bin
ifdef MPROGS
	#cp $(MPROGS) /usr/local/polarim/bin/macro
	cp $(MPROGS) /usr/local/cnipol/bin/macro
endif
