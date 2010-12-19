#SUBDIRS = burcmc libcmc rpoldaemon rhicpol tools cmcmtest emit
SUBDIRS = burcmc libcmc rpoldaemon rhicpol tools cmcmtest emit rhic2hbook

.PHONY: clean install subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	make -C $@ $(MAKECMDGOALS)

clean: subdirs

install: subdirs
