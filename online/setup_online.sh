export CDEV=/usr/local/cdev
export CDEVSHOBJ=${CDEV}/lib

#export CDEV_NAME_SERVER=acnlin07.pbn.bnl.gov
export CDEV_NAME_SERVER=acnserver01.pbn.bnl.gov
#export CDEVDDL=/usr/local/cnipol/online/cdev/PolarClient.ddl
export CDEVDDL=~/polarimeter/cnipol/online/cdev/PolarClient.ddl
export LD_LIBRARY_PATH=${CDEVSHOBJ}:${LD_LIBRARY_PATH}

source /usr/local/cern/root/bin/thisroot.sh
