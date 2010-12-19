CDEV      = /usr/local/cdev
CERN      = /usr/local/cern

#LIB_PATHS = -L$(CDEV)/store/X86/lib -L$(CDEV) -L$(CDEV)/Linux -L/usr/local/cern/lib
#LIB_PATHS = -L$(CDEV)/lib/Linux -L$(CERN)/2006b/lib
LIB_PATHS = -L$(CDEV)/store/X86/lib
#INC_PATHS = -I$(CDEV)/include -I$(CERN)/2006b/include/cfortran
INC_PATHS = -I$(CDEV)/store/X86/include/cdev -I$(CERN)/2006b/include/cfortran
LIBS      = -lcdev -lm -ldl -lstdc++ -lpthread
CXX       = g++
GCC       = gcc
FC        = f95

#SOFLAGS   = -static
FFLAGS    = -ffixed-line-length-none
CFLAGS    = $(INC_PATHS) -pthread
CXXFLAGS  = $(INC_PATHS) -pthread
LOADLIBES = $(LIB_PATHS) $(LIBS)

SRC_DIR       = .
INC_DIR       = .

HDRS       = $(wildcard $(INC_DIR)/*.h)
SRCS_CPP   = $(wildcard $(SRC_DIR)/*.cpp)
SRCS_C     = $(wildcard $(SRC_DIR)/*.c)
OBJS       = $(SRCS_CPP:.cpp=.o) $(SRCS_C:.c=.o)

#RHICPOL_OBJS = globals.o rpolutilp.o rhicpol.o rpolutil.o stat2str.o ../libcmc/libcmc.o
RHICPOL_OBJS = rpolutil.o test.o rhicpol.o stat2str.o ../libcmc/libcmc.o

target : rhicpol rhic2hbook sync2rtdltime rpolado sndpic printpos getCdevValue setCdevString sendFailResults

#rhicpol: globals.o rpolutilp.o rhicpol.o rpolutil.o stat2str.o ../libcmc/libcmc.o
#rhicpol: $(RHICPOL_OBJS)
test: $(RHICPOL_OBJS)
	$(CXX) $(SOFLAGS) $(RHICPOL_OBJS) $(LIB_PATHS) $(LIBS) -o $@
	@echo "$@ done"

rhic2hbook : rhic2hbook.o rdatautil.o rdatautilp.o stat2str.o
	$(FC) $^ -Wno-globals -o $@ $(LIB_PATHS) \
	 -lpacklib -lkernlib -lmathlib \
	 -lcdev -lm -ldl -lstdc++

rpolado : rpolado.o stat2str.o globals.o

sync2rtdltime : sync2rtdltime.o

sndpic : sndpic.o

printpos : printpos.o

getCdevValue : getCdevValue.o

setCdevString : setCdevString.o

sendFailResults : sendFailResults.o

%.o : %.c
	$(CXX) $(CFLAGS) -o $@ -c $<

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean :
	-rm -f rhicpol
	-rm -f rhic2hbook
	-rm -f jet2root
	-rm -f rpolado
	-rm -f sndpic
	-rm -f sync2rtdltime
	-rm -f printpos
	-rm -f getCdevValue
	-rm -f setCdevString
	-rm -f sendFailResults
	-rm -f *.o

