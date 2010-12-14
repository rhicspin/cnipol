PROGS = emitscan

include ../include/common.mk

CXXFLAGS += $(shell root-config --cflags)
LIBS += $(shell root-config --libs)

emitscan : emitscan.o emitsend.o
