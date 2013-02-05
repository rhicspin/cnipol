SUBDIRS = burcmc libcmc rpoldaemon rhicpol tools cmcmtest emit rhic2hbook

.PHONY: clean install subdirs createdir $(SUBDIRS)

all: subdirs
	@echo "all is done"

subdirs: $(SUBDIRS) createdir

$(SUBDIRS):
	make -C $@ $(MAKECMDGOALS)

clean: $(SUBDIRS)

cleanall: $(SUBDIRS)
	cd $(CNIPOL_DIR)/bin; rm -f $(PROGS)

install: subdirs
	@echo "install is done"

createdir:
	mkdir -p $(CNIPOL_DIR)/bin/macro
