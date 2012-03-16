CC=g++
cmcmtest : cmcmtest.o ../libcmc/libcmc.o

clean:
	-rm -f cmcmtest
	-rm -f *.o

cleanall: clean
	cd $(CNIPOL_DIR)/bin; rm -f cmcmtest

install:
	cp cmcmtest $(CNIPOL_DIR)/bin
