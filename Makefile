# 
# Oct 4, 2010 - Dmitri Smirnov 
#    - Created top level Makefile
# 

all: ana cnipol

clean: ana.clean cnipol.clean

cleanall: ana.cleanall cnipol.cleanall

ana: ana.all

ana.all:
	make -C ana all

ana.lib:
	make -C ana lib

ana.clean:
	make -C ana clean

ana.cleanall:
	make -C ana cleanall


cnipol: cnipol.all

cnipol.all: cnipol.lib cnipol.install

cnipol.lib:
	make -C src lib

cnipol.install:
	make -C src install

cnipol.clean:
	make -C src clean

cnipol.cleanall:
	make -C src cleanall


anapc:
	make -C anapc
