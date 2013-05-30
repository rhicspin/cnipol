#SUBDIRS = burcmc libcmc rpoldaemon rhicpol tools cmcmtest emit rhic2hbook
SUBDIRS = burcmc libcmc rpoldaemon rhicpol tools cmcmtest emit

ROOTLDFLAGS := $(shell root-config --nonew --ldflags)
ROOTLIBS    := $(shell root-config --libs)
CNIONL_LIB = $(CNIPOL_DIR)/lib/libcnionl.so

.PHONY: clean install subdirs createdir $(SUBDIRS)

all: subdirs $(CNIONL_LIB)
	@echo "all is done"

subdirs: $(SUBDIRS) createdir

$(SUBDIRS):
	make -C $@ $(MAKECMDGOALS)

$(CNIONL_LIB): rpoldaemon/rpoldata.o
	g++ -shared $(ROOTLDFLAGS) $(ROOTLIBS) $^ -o $@

clean: $(SUBDIRS)

cleanall: $(SUBDIRS)
	cd $(CNIPOL_DIR)/bin; rm -f $(PROGS)
	rm -f $(CNIONL_LIB)

install: subdirs
	@echo "install is done"

createdir:
	mkdir -p $(CNIPOL_DIR)/bin/macro
