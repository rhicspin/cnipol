PROGS = emitscan

include ../include/common.mk

CXXFLAGS = $(INC_PATHS) -pthread -Wall $(shell root-config --cflags)
LIBS += $(shell root-config --ldflags --libs)

emitscan : emitscan.o emitsend.o ../rpoldaemon/globals.o
