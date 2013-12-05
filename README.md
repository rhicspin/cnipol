
General Remarks
===============

The code to analyze the data from the RHIC polarimeteres has changed
significantly starting 2010. Nevertheless, one may still find it useful to look
through the older documentation which can be found at
http://www4.rcf.bnl.gov/~cnipol/Analysis/pC/index.html


How to Configure and Install cnipol
===================================

The best way to start with the analysis is to clone the latest version of the
code from the git repository. From the location where the package will be
installed on your machine issue the following command:

    git clone git@github.com:rhicspin/cnipol.git
    cd cnipol

To make the processing of the data controllable and predictable we create stable
releases of the code. The list of tagged stable releases can be viewed at
https://github.com/rhicspin/cnipol/releases One may switch from the development
version of the code checked out as above to the latest stable release by issuing
the following command:

    git checkout stable

Alternatively, one can checkout the latest stable release directly by doing:

    git clone -b stable git@github.com:rhicspin/cnipol.git
    cd cnipol

The analysis framework depends on a few external libraries and programs including

* ROOT
* CERNLIB
* MySQL
* MySQLPP
* CDEV

Before the code can be compiled and executed a number of environment variables
must be set. We provide an example bash script `script/setup.sh` with predefined
paths which should work on the eic cluster of the RACF (RHIC and ATLAS Computing
Facility). Review and modify this script as needed to match your specific system
configuration. To run this script with an optional argument for the RHIC run
number, e.g. `run_id = 9|11|12|13`, execute in the terminal:

    source script/setup.sh [run_id]

Once the environment variables are set the cnipol package can be build as:

    mkdir build
    cd build
    cmake ..
    make

The compiled executables should appear in the `cnipol/build` directory along
with the cnipol libraries.


How to Run cnipol
=================

Make sure the following environment variables are set to valid locations before
running the cnipol executables

    $CNIPOL_DIR
    $CNIPOL_DATA_DIR
    $CNIPOL_RESULTS_DIR

`asym` accepts a number of options which can be listed by running it with a `-h`
option. Here we just give a few most common examples used to process the regular
and calibration data.

To analyze a regular data file simply do:

    asym --calib --profile --use-db --no-update-db --raw-ext --asym --pmt --kinema -r <meas_id> &

where `<meas_id>` is the name of the run, e.g. 12345.001. Note that usually
`<file_name> = <meas_id>.data`. The program searches for the input <file_name>
file in $CNIPOL_DATA_DIR and creates an output directory <meas_id> in
$CNIPOL_RESULTS_DIR. Typically, the output directory contains a ROOT file with a
predefined set of histograms, a log file (stdoe.log) with the entire output of
the program, a file in PHP format with information about the measurement and
results to display on the web, and a file with calibration results.

To analyze an alpha or pulser run use the --alpha option:

    asym --alpha --use-db -r <meas_id>


How to produce the summary plots
================================

This section still has to be rewritten.
These instructions will work for executables in ~dsmirnov/cnipol on the eic
cluster.

    rsync -auv bluepc:/usr/local/polarim/data/*.data /eicdata/eic0005/run13/data/
    rsync -auv --include='*.root' --include='stdoe*' --include='runconfig.php' \
       --include='17???.???/' --exclude='*' pc2pc-phy:/usr/local/polarim/root/ \
       /eicdata/eic0005/run13/root/
    find /eicdata/eic0005/run13/data/ -regex \
       "/eicdata/eic0005/run13/data/\([0-9]+.[0-9]+\).data$" -printf "%f\n" | awk -F \
       ".data" '{print $1}' | sort > /eicdata/eic0005/runXX/lists/run13_all

Add polarization results from H-Jet web page to
`/eicdata/eic0005/runXX/hjet_pol`. Then get the intensity and other
information about the new fills from CDEV using the following script:

    ssh -Y acnlina "~/cnipol_trunk/script/get_fill_info.sh 17XXX" > \
       /eicdata/eic0005/runXX/cdev_info/cdev_17XXX

17XXX has to be replaced with the actual fill number. The summary plots are
produced with the following command:

    cd cnipol
    source setup13.sh
    ./bin/masym -m run13_phys -g


Other software packages for the RHIC polarimetry
================================================

Other packages are available from the SVN repository. Access to the repository
is restricted to BNL Domain NT accounts. An account can be requested at
http://www.bnl.gov/accounts/. The same account and password can be used to view
the repository on the web by visiting https://svn.bnl.gov/viewvc/.

{| class="wikitable cellpadded_top cntr"
! cnipol ([https://github.com/rhicspin/cnipol on github])
| Includes the DAQ code and the code for online data processing. Also includes
| the full offline analysis code for the p-Carbon polarimeters
|-
! ags_cni
| Code for AGS data analysis
|-
! jet
| Code for the H-jet polarimeter data analysis
|}


Build Status
============

[![Build Status](https://travis-ci.org/rhicspin/cnipol.png)](https://travis-ci.org/rhicspin/cnipol)
