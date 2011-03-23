PROGS = rhic2hbook
MPROGS = onliplot.kumac pvector.kumac pvector.pl lr_spinpat.pl rampplot.kumac

include ../include/common.mk

INC_PATHS += -I/usr/local/cern/2006b/include/cfortran
LIB_PATHS += -L/usr/local/cern/lib

rhic2hbook : rhic2hbook.o rdatautil.o rdatautilp.o
	$(FC) $^ -Wno-globals -o $@ $(LIB_PATHS) -lpacklib -lkernlib -lmathlib -lcdev -lm -ldl -lstdc++
