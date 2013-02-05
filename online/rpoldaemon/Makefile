PROGS = rpoldaemon 
IPROGS = rpoldaemon.sh rpolmeasure.sh failed.gif

include ../include/common.mk

CXXFLAGS += $(shell root-config --cflags)
CFLAGS += $(shell root-config --cflags)
LIBS += $(shell root-config --ldflags --libs)

rpoldaemon: rpoldaemon.o globals.o
