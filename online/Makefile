#SUBDIRS = burcmc libcmc rpoldaemon rhicpol tools cmcmtest emit
SUBDIRS = burcmc libcmc rpoldaemon rhicpol tools cmcmtest emit rhic2hbook

.PHONY: clean install subdirs mkdir $(SUBDIRS)

subdirs: $(SUBDIRS) mkdir

$(SUBDIRS):
	make -C $@ $(MAKECMDGOALS)

clean: $(SUBDIRS)

cleanall: $(SUBDIRS)
	cd $(CNIPOL_DIR)/bin; rm -f $(PROGS)

install: subdirs

mkdir:
	-mkdir $(CNIPOL_DIR)/bin/macro
