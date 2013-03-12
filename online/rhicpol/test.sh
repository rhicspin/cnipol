rhicpol -P -v200 -l mytest.log -i /usr/local/cnipol_trunk/config/polarimeter.blu1.alpha.ini -f mytest.data -d polarimeter.blu1 -c "a test" -t 5 -Ttest

rhicpol -P -v200 -i /usr/local/cnipol_trunk/config/polarimeter.blu1.alpha.ini -f mytest.data -d polarimeter.blu1 -t 5 -T test -g

emitscan -f /usr/local/polarim/data/99999.099.data -o mytest.root -p mytest.ps -d polarimeter.blu1
