[![Build Status](https://travis-ci.org/rhicspin/cnipol.svg?branch=master)](https://travis-ci.org/rhicspin/cnipol)

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Introduction](#introduction)
  - [General remarks](#general-remarks)
- [How to configure and install cnipol](#how-to-configure-and-install-cnipol)
- [How to run cnipol](#how-to-run-cnipol)
  - [Batch processing multiple data files](#batch-processing-multiple-data-files)
- [How to produce summary plots](#how-to-produce-summary-plots)
- [Other software packages for the RHIC polarimetry](#other-software-packages-for-the-rhic-polarimetry)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

Introduction
============

**cnipol** is a software package designed to acquire and analyze data from the
Relativistic Heavy Ion Collider (RHIC) polarimeters at the Brookhaven National
Lab (BNL). The first objective of this package is usually referred to as the
"online" layer and designed to support the data acquisition and to provide
a quick feedback by carrying out a fast online analysis. The second objective is to
execute a more rigorous "offline" data analysis and present the results in
a systematic way. It is normally referred to as the "offline" layer of
**cnipol**.

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
installed on your machine (such as eic0005)issue the following command:

    git clone https://github.com/rhicspin/cnipol.git
    cd cnipol

If you plan to push changes back to the repository a github account is required.
The source code in this case can be cloned via ssh protocol:

    git clone git@github.com:rhicspin/cnipol.git
    cd cnipol


The analysis framework depends on a few external libraries and programs including

* ROOT
* MySQL

The data acquisition and online processing code depends on CERNLIB and CDEV.

Before the code can be compiled and executed a number of environment variables
must be set. We provide an example bash script `script/setup.sh` with predefined
paths which should work on the eic cluster of the RACF (RHIC and ATLAS Computing
Facility). Review and modify this script as needed to match your specific system
configuration. This script can be ran with an optional argument for the RHIC run
number, e.g. `run_id = 9|11|12|13|15|17`, execute in the terminal (Note: You need to be bash shell. Type bash):

    source script/setup.sh [run_id]

Once the environment variables are set the cnipol package can be built in
$CNIPOL_DIR as:

    mkdir build
    cd build
    cmake28 .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
    make

The compiled executables should appear in this `build` directory along
with the cnipol libraries.


How to run cnipol
=================

Make sure the following environment variables are set to valid locations before
running the cnipol executables

    $CNIPOL_DATA_DIR
    $CNIPOL_RESULTS_DIR

`asym` accepts a number of options which can be listed by running it with a `-h`
option. Here we just give a few most common examples used to process regular and
calibration data.

To analyze a regular data file simply do in the build directory:

    asym -r <meas_id>

where `<meas_id>` is a unique name of a measurement, e.g. 12345.001. Note that
usually `<file_name> = <meas_id>.data`. The program searches for the input
<file_name> file in $CNIPOL_DATA_DIR and creates an output directory <meas_id>
in $CNIPOL_RESULTS_DIR. Typically, the output directory contains a ROOT file
with a predefined set of histograms, a log file (stdoe.log) with the entire
output of the program, a PHP file with information about the measurement and
results to display on the web, and a file with calibration results.

To analyze an alpha or pulser measurement use the --alpha option:

    asym --alpha -r <meas_id>


Batch processing multiple data files
------------------------------------

Once in a while, when, for example, the **cnipol** code is modified one may need
to re-analyze a large number of data files. This can be achieved in several ways
by utilizing common linux tools. Here for completeness we provide a simple shell
script that takes a text file containing a list of measurements (one `<meas_id>`
per line) as a single argument and starts a certain number of parallel jobs in
the background. The scripts for energy calibration and regular runs can be
executed respectively as:

    script/run_asym_alpha.sh <meas_list>
    script/run_asym.sh <meas_list>


How to produce summary plots
============================

Often it is useful to see how polarization or other observables change from
measurement to measurement during some period of time. To study such time
dependence we developed `masym`, a program that runs on a set of measurements
analyzed with `asym`. The output ROOT files produced by `asym` are used as input
to `masym`. Executing the following command will create a `<meas_list>` directory
in $CNIPOL_RESULTS_DIR with a ROOT file containing summary histograms:

    masym -m <meas_list>

Here <meas_list> is a plain text file with a list of measurements to process.
This file is assumed to exist in $CNIPOL_RESULTS_DIR/runXX/lists/ and to have
one `<meas_id>` per line.

Optionally, the analyzer can provide a plain text file
$CNIPOL_RESULTS_DIR/runXX/hjet_pol with H-Jet polarization values, and one file
$CNIPOL_RESULTS_DIR/runXX/cdev_info/cdev_NNNNN per fill NNNNN containing
intensity and other beam parameters. Information from these text files will be
used by `masym` in the summary plots. Information from CDEV can be extracted
using the following script on the acnlin CAD cluster:

    $CNIPOL_DIR/script/get_fill_info.sh <NNNNN>

where <NNNNN> must be replaced with an actual fill number.

Similarly to `masym` we provide a program to study time dependences in a set of
alpha-calibration measurements. This program is called `malpha` and can be
invoked as:

    malpha -m <meas_list>

where `<meas_list>` is a file in `$CNIPOL_RESULTS_DIR/runXX/lists/` with a list
of unique names corresponding to measurements recorded by the polarimeters when
no beam was present in the RHIC.

General Description of Directory Layout
=======================================

There are 7 major directories in the cnipol package:

cnipol/inc/ -- This directory contains all the source code header files

cnipol/src/ -- This directory contains all the cpp files

cnipol/build/ -- This is build directory to compile the code using make 

cnipol/webview/ --  This directory contains all the code relavent for the webpage

cnipol/script/ -- This directory contains all the scripts that can quickly allow a user to run the code

eicdata/eic0005/runXX/ -- This directory contains lists. The list of measuements, the list of target status, and the lists from cdev.  

eicdata/eic0005/run17/ -- This directory holds all the data after processing through asym (Using 17 as an example, could be any other run number).

Automating the Process
======================

First, you need to follow the link below to set up a tunnel to the blue and yellow pC machines at IP 12. This will provide you with a tunnel into the machines without requiring a password. 

https://wiki.bnl.gov/rhicspin/Automated_pC_setup

After, the tunnel has been established you can start the automated process by executing the following command: 
    
    nohup ./run_asym_daemon.sh > some_output_file_name

and you can put an ampersand at the end of the process if you want to hide it. The ampersand will allow you to continue working in your current terminal. Regardless, the process will be running and looking for new files every 60 seconds. 


Other polarimetry software packages
===================================

Different people have been working on analysing polarimetry data. Some notable
packages used for this purpose are:

<table>
	<tr>
		<th>ags_cni</th>
		<td>
			<p>Code for AGS data analysis.</p>
			<p>AGS has a jet polarimeter much like the one at RHIC.
			Analysis code has branched a long time ago and is now supported by CAD.</p>
		</td>
	</tr>
	<tr>
		<th>hjet-analysis</th>
		<td>
			<p>Code for the H-jet polarimeter data analysis</p>
			<p>Supported by the RHIC Spin group. As many other analyses
			it's reader code is based on the code found in <a href="https://github.com/rhicspin/polarim-online/blob/master/rhic2hbook/rdatautil.cpp">rdatautil.c</a>.</p>
		</td>
	</tr>
</table>

<!--
# vim: set noexpandtab:
-->
