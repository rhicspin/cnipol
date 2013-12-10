Introduction
============

**cnipol** is a software package designed to acquire and analyze data from the
Relativistic Heavy Ion Collider (RHIC) polarimeters at the Brookhaven National
Lab (BNL). The first objective is usually referred to as the 'online' part of
cnipol which is designed to support the data acquisition and the second
objective is referred to as the 'offline' data analysis.

There is a wiki page with some details about the RHIC polarimetry:

https://wiki.bnl.gov/rhicspin/Polarimetry

The entry page with links to results obtained with cnipol can be found at

http://www.phy.bnl.gov/cnipol/


General remarks
---------------

The code for the analysis of the RHIC polarimeters data has changed
significantly starting 2010. Nevertheless, one may still find it useful to look
through the older documentation which can be found at

http://www4.rcf.bnl.gov/~cnipol/Analysis/pC/index.html


How to configure and install cnipol
===================================

The best way to start with the analysis is to clone the latest version of the
code from the git repository. From the location where the package will be
installed on your machine issue the following command:

    git clone git@github.com:rhicspin/cnipol.git
    cd cnipol

To make the processing of the data controllable and predictable we create stable
releases of the code. The list of tagged stable releases can be viewed at
https://github.com/rhicspin/cnipol/releases
One may switch from the development version of the code checked out as above to
the latest stable release by issuing the following command:

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


How to run cnipol
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

    asym -r <meas_id>

where `<meas_id>` is a unique name of a measurement, e.g. 12345.001. Note that
usually `<file_name> = <meas_id>.data`. The program searches for the input
<file_name> file in $CNIPOL_DATA_DIR and creates an output directory <meas_id>
in $CNIPOL_RESULTS_DIR. Typically, the output directory contains a ROOT file
with a predefined set of histograms, a log file (stdoe.log) with the entire
output of the program, a file in PHP with information about the measurement and
results to display on the web, and a file with calibration results.

To analyze an alpha or pulser measurement use the --alpha option:

    asym --alpha -r <meas_id>


How to produce summary plots
============================

Often it is useful to see how polarization or other observables change from
measurement to measurement during some period of time. To study such time
dependence we developed `masym`, a program that runs on a set of measurements
analyzed with `asym`. The output ROOT files produced by `asym` are used as input
to `masym`. Executing the following command will create a <meas_list> directory
in $CNIPOL_RESULTS_DIR with a ROOT file containing summary histograms:

    masym -m <meas_list>

Here <meas_list> is a plain text file with a list of measurements to process.
This file is assumed to exist in $CNIPOL_RESULTS_DIR/runXX/lists/ and to have
one <meas_id> per line.

Optionally, the analyzer can provide a plain text file
$CNIPOL_RESULTS_DIR/runXX/hjet_pol with H-Jet polarization values, and one file
$CNIPOL_RESULTS_DIR/runXX/cdev_info/cdev_NNNNN per fill NNNNN containing
intensity and other beam parameters. Information from these text files will be
used by `masym` in the summary plots. Information from CDEV can be extracted
using the following script on the acnlin CAD cluster:

    $CNIPOL_DIR/script/get_fill_info.sh NNNNN

where NNNNN must be replaced with an actual fill number.


Other software packages for the RHIC polarimetry
================================================

Other packages are available from the BNL SVN repository. Access to the
repository is restricted to BNL Domain NT accounts. An account can be requested
at http://www.bnl.gov/accounts/. The same account and password can be used to
view the repository on the web by visiting https://svn.bnl.gov/viewvc/.

<table>
	<tr>
		<th>cnipol</th>
		<td>
This is an older unsupported version of this package.<br/>
For the up-to-date version see https://github.com/rhicspin/cnipol
		</td>
	</tr>
	<tr>
		<th>ags_cni</th>
		<td>Code for AGS data analysis</td>
	</tr>
	<tr>
		<th>jet</th>
		<td>Code for the H-jet polarimeter data analysis</td>
	</tr>
</table>


Build Status
============

[![Build Status](https://travis-ci.org/rhicspin/cnipol.png)](https://travis-ci.org/rhicspin/cnipol)
