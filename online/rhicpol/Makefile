###PROGS = rhicpol
###
###include ../include/common.mk
###
###rhicpol : rpolutilp.o rhicpol.o rpolutil.o ../libcmc/libcmc.o ../rpoldaemon/globals.o ../rpoldaemon/rpoldata.o
###

include /vobs/libs/makefiles/MakeStd.inc

NAME = rhicpol

USESOLIBS = yes
INC = -I../include
USER_CXXFLAGS = $(shell root-config --cflags)
LDFLAGS = ../libcmc/libcmc.o $(shell root-config --libs)

PROG1 = rhicpol
SRCS1 = rhicpol.cpp rpolutil.cpp rpolutilp.cpp globals.cpp rpoldata.cpp
PRIVATE_HEADERS1 = rpolutil.h
LIBS1 = cdevCns cdev includeCdev GenData rhicError Async setHist

include $(MAKEDIR)/MakeApp.inc
