#!/bin/csh

setup 64b
  
set RUN_ID="22"

if ( $#argv > 0) then 
  set RUN_ID=$argv[1]
endif

#RUN_ID=$(printf '%02d' $RUN_ID)

#RUN_ID=13

echo "Setting environment variables for RUN_ID=${RUN_ID}:"

# ROOT
#setenv BOOST_ROOT /afs/rhic/x8664_sl6/opt/star/sl64_x8664_gcc482
#setenv PATH=/afs/rhic/rcassoft/x8664_sl6/gcc492/bin:$PATH
#setenv LD_LIBRARY_PATH=/afs/rhic/rcassoft/x8664_sl6/gcc492/lib64:$LD_LIBRARY_PATH
#source /eicdata/eic0005/root_gcc49/bin/thisroot.sh
#setenv PATH /afs/rhic/rcassoft/x8664_sl6/gcc482/bin:$PATH
#setenv LD_LIBRARY_PATH /afs/rhic/rcassoft/x8664_sl6/gcc482/lib64:$LD_LIBRARY_PATH
#source /afs/rhic.bnl.gov/star/ROOT/5.34.38/.sl64_gcc482/rootdeb/bin/thisroot.csh

# CERNLIB (required for online programs)
setenv CERNLIB_DIR /cern/2006b
setenv PATH $PATH\:${CERNLIB_DIR}/bin

# MySQL
#setenv MYSQL_LIB_DIR=/usr/lib/mysql
#setenv MYSQLPP_LIB_DIR=/eicdata/eic0005/mysql++
#setenv LD_LIBRARY_PATH $LD_LIBRARY_PATH\:$MYSQL_LIB_DIR
#setenv LD_LIBRARY_PATH $LD_LIBRARY_PATH\:$MYSQLPP_LIB_DIR
#echo "\$MYSQL_LIB_DIR=$MYSQL_LIB_DIR"
#echo "\$MYSQLPP_LIB_DIR=$MYSQLPP_LIB_DIR"

setenv CNIPOL_DB_NAME cnipol
setenv CNIPOL_DB_HOST eicsrv1.sdcc.bnl.gov
setenv CNIPOL_DB_USER cnipol
setenv CNIPOL_DB_PASSWORD \(n\!P0l
echo $CNIPOL_DB_PASSWORD

# CNIPOL
#setenv CNIPOL_DIR=~/pCpol/cnipol
setenv CNIPOL_DIR /star/u/zchang/run22/cnipol
setenv CNIPOL_DATA_DIR /gpfs02/eic/cnipol/zchang/run$RUN_ID/data
#setenv CNIPOL_DATA_DIR /gpfs02/eic/cnipol/run17/data
#setenv CNIPOL_RESULTS_DIR /eicdata/eic0005/run$RUN_ID/root_rp
setenv CNIPOL_RESULTS_DIR /gpfs02/eic/cnipol/zchang/run$RUN_ID/root

setenv PATH $PATH\:$CNIPOL_DIR/build
setenv LD_LIBRARY_PATH $LD_LIBRARY_PATH\:$CNIPOL_DIR/build

# UTILS
setenv UTILS_LIB_DIR $CNIPOL_DIR/build/contrib/root-helper/utils/

setenv LD_LIBRARY_PATH $LD_LIBRARY_PATH\:$UTILS_LIB_DIR

# CDEV
#setenv CDEV_DIR /usr/local/cdev
#setenv CDEVDDL ${CNIPOL_DIR}/online/cdev/PolarClient.ddl
#setenv CDEV_NAME_SERVER acnserver01.pbn.bnl.gov

#setenv LD_LIBRARY_PATH $LD_LIBRARY_PATH\:${CDEV_DIR}/lib
#echo "\$CDEV_DIR=$CDEV_DIR"
#echo "\$CDEVDDL=$CDEVDDL"
#echo "\$CDEV_NAME_SERVER=$CDEV_NAME_SERVER"
echo
echo "ROOTSYS=$ROOTSYS"
echo "CERNLIB_DIR=$CERNLIB_DIR"
echo "CNIPOL_DIR=$CNIPOL_DIR"
echo "CNIPOL_DATA_DIR=$CNIPOL_DATA_DIR"
echo "CNIPOL_RESULTS_DIR=$CNIPOL_RESULTS_DIR"
echo
echo "UTILS_LIB_DIR=$UTILS_LIB_DIR"
echo
echo "PATH=$PATH"
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
