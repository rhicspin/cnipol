#rate_check.sh
#Feb. 14, 2007
#I. Nakagawa
#This is a script to see the correlation between rate1 (12C_banana/(t_stop-t_start))
#and rate2 (12C_banana/ndelim)


grep -v Junk bkp/070117/OfflinePol_Blue_phys.dat | gawk '{print $1, $19}' > aaa.dat
grep -v Junk bkp/070214/OfflinePol_Blue_phys.dat | gawk '{print $1, $19}' > bbb.dat

paste aaa.dat bbb.dat > ccc.dat
NLINE=`wc ccc.dat | gawk '{print $1-4}'`
tail -n $NLINE ccc.dat > blue_rate.dat


grep -v Junk bkp/070117/OfflinePol_Yellow_phys.dat | gawk '{print $1, $19}' > aaa.dat
grep -v Junk bkp/070214/OfflinePol_Yellow_phys.dat | gawk '{print $1, $19}' > bbb.dat

paste aaa.dat bbb.dat > ccc.dat
NLINE=`wc ccc.dat | gawk '{print $1-4}'`
tail -n $NLINE ccc.dat > yellow_rate.dat

rm -rf aaa.dat bbb.dat ccc.dat

echo "output file: blue_rate.dat, yellow_rate.dat"




