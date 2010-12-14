SUBDIRS = burcmc libcmc rpoldaemon rhicpol tools cmcmtest emit

.PHONY: clean install subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	make -C $@ $(MAKECMDGOALS)

clean: subdirs

install: subdirs
