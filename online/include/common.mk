
CDEV=/usr/local/cdev

LIB_PATHS = -L$(CDEV)/store/X86/lib
INC_PATHS = -I$(CDEV)/store/X86/include/cdev -I../include -I/usr/include
LIBS = -lcdev -lm -ldl -lstdc++ -lpthread

FFLAGS = -ffixed-line-length-none
CFLAGS = $(INC_PATHS) -pthread -Wall
CXXFLAGS = $(INC_PATHS) -pthread -Wall
LOADLIBES = $(LIB_PATHS) $(LIBS)
FC = f95
CXX = g++
CC = g++

all: $(PROGS)

clean:
	-rm -f $(PROGS)
	-rm -f *.o

cleanall: clean
	cd $(CNIPOL_DIR)/bin; rm -f $(PROGS) $(IPROGS); cd $(CNIPOL_DIR)/bin/macro; rm -f $(MPROGS);
	cd $(CNIPOL_DIR)/bin;
	-rmdir macro

install:
	cp $(PROGS) $(IPROGS) $(CNIPOL_DIR)/bin

#cp $(PROGS) $(IPROGS) /usr/local/cnipol/bin

ifdef MPROGS
	cp $(MPROGS) $(CNIPOL_DIR)/bin/macro
endif
