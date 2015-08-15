#!/bin/sh

set -e

if [ -z "$CNIPOL_DATA_DIR" ]; then
        echo "Please set \$CNIPOL_DATA_DIR"
        exit 1
fi
if [ -z "$CNIPOL_DATA_DIR" ]; then
        echo "Please set \$CNIPOL_RESULTS_DIR"
        exit 1
fi

LIST_FILE="$CNIPOL_RESULTS_DIR/runXX/lists/run15_alpha"

cat $LIST_FILE | awk '{print "'$CNIPOL_RESULTS_DIR'/" $0}' | xargs rm -rfv
rm -rf $CNIPOL_RESULTS_DIR/run15_alpha_pp_malpha || true
rm -rf $CNIPOL_RESULTS_DIR/run15_alpha_pA_malpha || true
mkdir $CNIPOL_RESULTS_DIR/run15_alpha_pp_malpha
mkdir $CNIPOL_RESULTS_DIR/run15_alpha_pA_malpha

cat $LIST_FILE | parallel -j20 "CNIPOL_DATA_DIR=$CNIPOL_DATA_DIR CNIPOL_RESULTS_DIR=$CNIPOL_RESULTS_DIR nice -5 ionice -c 3 ~/cnipol/build/asym --alpha -r "
./malpha -m run15_alpha_pp_malpha -o /dev/null -g --image-format tex
./malpha -m run15_alpha_pA_malpha -o /dev/null -g --image-format tex
