# File name           : makefile
#
# Author              : Hiromi Okada 
# Creation data       : 2004/Dec

ROOTSYS = /usr/local/cern/root
ROOTINC 	= -I$(ROOTSYS)/include
ROOTLIB		=-L$(ROOTSYS)/lib -lCore -lCint -lHist -lGraf -lGraf3d \
		-lGpad -lMinuit	-lTree -lRint -lPostscript -lMatrix \
		-lPhysics -lm -ldl -lpthread -rdynamic -lstdc++

LIB_PATHS = -L/lib -L/usr/lib -L/usr/X11R6/lib -L/usr/local/lib -L/usr/local/cern/lib
INC_PATHS = -I. -I/usr/include
LIBS =  -lpacklib -lkernlib -lmathlib -lpawlib -lgraflib -lgrafX11 -lm -lc


CC		= g++
FC		= g77
#CFLAGS		= -O2 -pg -g -Wall
#CFLAGS		= -O2 -Wall
CFLAG		= -O2 -Wno-deprecated
FLIB		= -lg2c -lgcc -lm -lnsl

D_FLAGS = -Df2cFortran
#F77 = g77

## Objects declare
OBJS		= rhic_offline.o rhic_process.o rhic_asymmetry.o rhic_util.o \
		event_pW.o event_pN.o get_ats.o FitWaveform.o FitMASK.o \
		InputVector.o hb_lib.o event_pNroot.o get_missing_mass.o\
                recoil_mass.o jet_process.o GetBanana.o get_8elements.o \
                elements2asym.o FitWaveMask.o setChan.o CutBadWf.o IncidentE.o  
PROGRAM		= Jet_online



$(PROGRAM) : $(OBJS)
	$(CC) $(CFLAG) -o $@ $(OBJS) $(LIB_PATHS) $(INC_PATHS) $(ROOTLIB) $(LIBS) $(FLIB) 


######## OBJECT ##################

rhic_offline.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

rhic_process.o :   rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

rhic_asymmetry.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

rhic_util.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

event_pW.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

event_pN.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

event_pN_RAW.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

get_ats.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

FitWaveform.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

FitMASK.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

InputVector.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

event_pNroot.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

get_missing_mass.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

jet_process.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

GetBanana.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

get_8elements.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

elements2asym.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

FitWaveMask.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

setChan.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

CutBadWf.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h

IncidentE.o :  rhic_offline.h rhicpol.h rpoldata.h rhic_class.h





.C.o:
	$(CC) $(CFLAG) -c -I. $(ROOTINC) $(D_FLAGS) $<

.f.o:
	$(FC) $(CFLAG) -c $<

##############################
clean :
	rm -f $(OBJS) core






