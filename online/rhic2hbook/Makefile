PROGS = rhic2hbook
MPROGS = onliplot.kumac pvector.kumac pvector.pl lr_spinpat.pl rampplot.kumac

include ../include/common.mk

# override the install target from common.mk
install:
	cp -f $(PROGS) $(IPROGS) $(CNIPOL_DIR)/bin
	-ln -s `pwd`/test.sh $(CNIPOL_DIR)/bin/analyze.sh
ifdef MPROGS
	cp $(MPROGS) $(CNIPOL_DIR)/bin/macro
endif

CERNLIB_DIR=/usr/local/cern
INC_PATHS += -I$(CERNLIB_DIR)/include/cfortran
LIB_PATHS += -L$(CERNLIB_DIR)/lib

CXXFLAGS += $(shell root-config --cflags)
CFLAGS += $(shell root-config --cflags)
LIBS += $(shell root-config --ldflags --libs)

rhic2hbook : rhic2hbook.o rdatautilp.o rdatautil.o
	$(FC) $^ -Wno-globals -o $@ $(LIB_PATHS) -lpacklib -lkernlib -lmathlib -lcdev -lm -ldl -lstdc++ $(LIBS)
