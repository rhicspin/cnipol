source $CNIPOL_DIR/script/run_asym/bgx_asym.sh

# Run 12 alpha calibration files
bgx_limit 10 ./bin/asym -r 170112.blu1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 170112.yel1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 170112.blu2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 170112.yel2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 180112.blu1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 180112.yel1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 180112.blu2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 180112.yel2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 190112.blu1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 190112.yel1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 190112.blu2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 190112.yel2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 200112.blu1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 200112.yel1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 200112.blu2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 200112.yel2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 090212.blu1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 090212.yel1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 090212.blu2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 090212.yel2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 150212.blu1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 150212.yel1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 150212.blu2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 150212.yel2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 290212.blu1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 290212.yel1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 290212.blu2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 290212.yel2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 140312.blu1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 140312.yel1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 140312.blu2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 140312.yel2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 280312.blu1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 280312.yel1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 280312.blu2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 280312.yel2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 190412.blu1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 190412.yel1.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 190412.blu2.alpha0 --alpha --use-db -g --copy
bgx_limit 10 ./bin/asym -r 190412.yel2.alpha0 --alpha --use-db -g --copy


# Run 11 alpha calibration files
#bgx_limit 10 ./bin/asym -r test.blu1.004 --alpha --use-db --pol-id=0 -g --copy
#bgx_limit 10 ./bin/asym -r test.yel1.005 --alpha --use-db --pol-id=1 -g --copy
#bgx_limit 10 ./bin/asym -r test.blu2.005 --alpha --use-db --pol-id=2 -g --copy
#bgx_limit 10 ./bin/asym -r test.yel2.004 --alpha --use-db --pol-id=3 -g --copy
#bgx_limit 10 ./bin/asym -r 250211.blu1.alpha0 --alpha --use-db -g --copy
#bgx_limit 10 ./bin/asym -r 250211.yel1.alpha0 --alpha --use-db -g --copy
#bgx_limit 10 ./bin/asym -r 250211.blu2.alpha0 --alpha --use-db -g --copy
#bgx_limit 10 ./bin/asym -r 250211.yel2.alpha0 --alpha --use-db -g --copy


# Run 9 alpha calibration files. Need to source run 9 setup.sh
#bgx_limit 10 ./bin/asym -r alpha0429.000 --alpha --use-db --pol-id=0 -g --copy
#bgx_limit 10 ./bin/asym -r alpha429.100 --alpha --use-db --pol-id=1 -g --copy
#bgx_limit 10 ./bin/asym -r alpha0429.200 --alpha --use-db --pol-id=2 -g --copy
#bgx_limit 10 ./bin/asym -r alpha429.300 --alpha --use-db --pol-id=3 -g --copy

bgx_wait
