source $CNIPOL_DIR/script/run_asym/bgx_asym.sh

#bgx_limit 3 sleep 10

FILE_RUNS=$1

exec < $FILE_RUNS

while read run_name
do
   #echo "bgx_limit 16 ./bin/asym -r $run_name --calib --profile --use-db";
   #echo "bgx_limit 16 ./bin/asym -r $run_name --calib --profile --use-db -g --copy";
   #bgx_limit 16 ./bin/asym -r $run_name --calib --profile --use-db -g --copy
   #bgx_limit 16 ./bin/asym -r $run_name --calib --profile --use-db --copy
   #bgx_limit 16 ./bin/asym -r $run_name --calib --profile --use-db --pmt --raw-ext --asym --kinema -g --copy 
   #bgx_limit 16 ./bin/asym -r $run_name --calib --profile --use-db -g --copy --raw --asym --disable-det=101101 --sfx=hama
   bgx_limit 10 $CNIPOL_DIR/bin/asym -r $run_name --calib --profile --use-db --raw-ext --asym --pmt --kinema -g
done

# Wait until all others are finished.
bgx_wait
