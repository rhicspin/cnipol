# 
# 
# Oct 4, 2010 - Dmitri Smirnov 
#    - Created top level Makefile
# 

all: ana

ana: ana.all

ana.all:
	make -C ana all

ana.lib:
	make -C ana lib

ana.clean:
	make -C ana clean

anapc:
	make -C anapc
