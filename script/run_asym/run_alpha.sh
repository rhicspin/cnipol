source ~/run/bgx_asym.sh

# Run 12 alpha calibration files
bgx_limit 16 ./bin/asym -r 190112.blu1.alpha0 --alpha --use-db -g --copy
bgx_limit 16 ./bin/asym -r 190112.yel1.alpha0 --alpha --use-db -g --copy
bgx_limit 16 ./bin/asym -r 190112.blu2.alpha0 --alpha --use-db -g --copy
bgx_limit 16 ./bin/asym -r 190112.yel2.alpha0 --alpha --use-db -g --copy

# Run 11 alpha calibration files
#bgx_limit 16 ./bin/asym -r test.blu1.004 --alpha --use-db --pol-id=0 -g --copy
#bgx_limit 16 ./bin/asym -r test.yel1.005 --alpha --use-db --pol-id=1 -g --copy
#bgx_limit 16 ./bin/asym -r test.blu2.005 --alpha --use-db --pol-id=2 -g --copy
#bgx_limit 16 ./bin/asym -r test.yel2.004 --alpha --use-db --pol-id=3 -g --copy
#bgx_limit 16 ./bin/asym -r 250211.blu1.alpha0 --alpha --use-db -g --copy
#bgx_limit 16 ./bin/asym -r 250211.yel1.alpha0 --alpha --use-db -g --copy
#bgx_limit 16 ./bin/asym -r 250211.blu2.alpha0 --alpha --use-db -g --copy
#bgx_limit 16 ./bin/asym -r 250211.yel2.alpha0 --alpha --use-db -g --copy

# Run 9 alpha calibration files. Need to source run 9 setup.sh
#bgx_limit 16 ./bin/asym -r alpha0429.000 --alpha --use-db --pol-id=0 -g --copy
#bgx_limit 16 ./bin/asym -r alpha429.100 --alpha --use-db --pol-id=1 -g --copy
#bgx_limit 16 ./bin/asym -r alpha0429.200 --alpha --use-db --pol-id=2 -g --copy
#bgx_limit 16 ./bin/asym -r alpha429.300 --alpha --use-db --pol-id=3 -g --copy

bgx_wait
