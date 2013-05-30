PROGS = rhicpol

include ../include/common.mk

#CXXFLAGS += $(shell root-config --cflags)
#CFLAGS += $(shell root-config --cflags)
#LIBS += $(shell root-config --ldflags --libs)

rhicpol : rpolutilp.o rhicpol.o rpolutil.o ../libcmc/libcmc.o ../rpoldaemon/globals.o ../rpoldaemon/rpoldata.o
