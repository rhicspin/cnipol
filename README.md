[![Build Status](https://travis-ci.org/rhicspin/cnipol.svg?branch=master)](https://travis-ci.org/rhicspin/cnipol)

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Introduction](#introduction)
- [How to configure and install cnipol](#how-to-configure-and-install-cnipol)
  - [Obtaining the code](#obtaining-the-code)
  - [Syncing changes with central repository](#syncing-changes-with-central-repository)
  - [Compiling the code](#compiling-the-code)
- [How to run cnipol](#how-to-run-cnipol)
  - [Batch processing multiple data files](#batch-processing-multiple-data-files)
- [How to produce summary plots](#how-to-produce-summary-plots)
- [General Description of Directory Layout](#general-description-of-directory-layout)
- [Automating the Process](#automating-the-process)
- [Alpha Calibration File](#alpha-calibration-file)
- [General remarks](#general-remarks)
  - [Other polarimetry software packages](#other-polarimetry-software-packages)

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

The results obtained with cnipol can be found at

http://www.phy.bnl.gov/cnipol/


How to configure and install cnipol
===================================

The development of cnipol is done using the git version control system. Good manuals can be found at:

 * https://git-scm.com/book/en/v2 - somewhat in-depth
 * https://help.github.com — github specifics

Obtaining the code
------------------

The best way to start with the analysis is to clone the latest version of the
code from the git repository. From the location where the package will be
installed on your machine issue a following command:

    git clone https://github.com/rhicspin/cnipol.git
    cd cnipol
    
This will create a cnipol directory with your local cnipol repository.

Syncing changes with central repository
---------------------------------------

In order to sync your changes in your local copy with the central repository you will need to have have them commited to your local repository first. To do that you need to first "stage" them using

    git add -p

This will interactively ask you which changes you want to stage. If there are new files that you would like to add to the repository use

    git add path/to/new/file
    
To check what changes are staged use ```git status``` and ```git diff --staged```.
To unstage all of the changes use ```git reset```.
After staging the changes use ```git commit -m "your commit message here"``` to turn them into a commit. Use ```git log -p``` to see list of local commits.

The newly created commit will only exist in your local repository. To synchronize with the common repository issue:

    git pull --rebase
    git push
    
Later should ask for your github credentials. If you plan to push oftenб it is a good idea to generate an ssh key and add it to gihub: https://help.github.com/articles/connecting-to-github-with-ssh/
    
It is important to sync your changes in a timely manner. Letting your changes pile locally for too long increases chance of having a merge conflict.

Compiling the code
------------------

The analysis framework depends on a few external libraries and programs including

* ROOT5
* MySQL
* sqlite3
* boost (through opencdev)

These (except ROOT) are typically readily available on most linux systems.

Before the code can be compiled and executed a number of environment variables
must be set. We provide an example bash script `script/setup.sh` with predefined
paths which should work on the eic cluster of the RACF (RHIC and ATLAS Computing
Facility). Review and modify this script as needed to match your specific system
configuration. An example is the variable `CNIPOL_DIR`, which the directory you setup the code. This script can be ran with an optional argument for the RHIC run
number, e.g. `run_id = 9|11|12|13|15|17`, execute in the terminal (Note: You need to be in a bash shell. Type bash):

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

    ./asym -r <meas_id>

where `<meas_id>` is a unique name of a measurement, e.g. 12345.001. Note that
usually `<file_name> = <meas_id>.data`. The program searches for the input
<file_name> file in $CNIPOL_DATA_DIR and creates an output directory <meas_id>
in $CNIPOL_RESULTS_DIR. Typically, the output directory contains a ROOT file
with a predefined set of histograms, a log file (stdoe.log) with the entire
output of the program, a PHP file with information about the measurement and
results to display on the web, and a file with calibration results.

To analyze an alpha or pulser measurement use the --alpha option:

    ./asym --alpha -r <meas_id>


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

Before you begin you should source the setup file as decribed above. 

Often it is useful to see how polarization or other observables change from
measurement to measurement during some period of time. To study such time
dependence we developed `masym`, a program that runs on a set of measurements
analyzed with `asym`. The output ROOT files produced by `asym` are used as input
to `masym`. Executing the following command will create a `<meas_list>` directory
in $CNIPOL_RESULTS_DIR with a ROOT file containing summary histograms:

    ./masym -m <meas_list>

Here <meas_list> is a plain text file with a list of measurements to process.
This file is assumed to exist in $CNIPOL_RESULTS_DIR/runXX/lists/ and to have
one `<meas_id>` per line.

Optionally, the analyzer can provide a plain text file
$CNIPOL_RESULTS_DIR/runXX/hjet_pol with H-Jet polarization values.
Information from these text files will be
used by `masym` in the summary plots.

Similarly to `masym`, we provide a program to study time dependences in a set of
alpha-calibration measurements. This program is called `malpha` and can be
invoked as:

    ./malpha -m <meas_list> -o <output_file>

where `<meas_list>` is a file in `$CNIPOL_RESULTS_DIR/runXX/lists/` with a list
of unique names corresponding to measurements recorded by the polarimeters when
no beam was present in the RHIC.

General Description of Directory Layout
=======================================

 * ```$CNIPOL_DATA_DIR``` — directory containing raw input data files that are read by `asym`. Typically points to ```/eicdata/eic0005/run??/data``` where ```??``` is the run number.
 * ```$CNIPOL_RESULTS_DIR``` — output directory for asym (it creates ```$CNIPOL_RESULTS_DIR/<meas_id>``` subdirectories). Also input and output directory for masym (it reads asym output and produces its output in ```$CNIPOL_RESULTS_DIR/<meas_list>```). Typically points to ```/eicdata/eic0005/run??/root```.
 * Another input directory for masym is ```/eicdata/eic0005/runXX``` (literally ```XX```) that is a local git repository clone of https://github.com/rhicspin/runXX. It contains plain text files with lists of measurements, target statuses and H-Jet polarizations. I also contains text cdev logs, but these are not used anymore. Due to some bug masym will look for it in ```$CNIPOL_RESULTS_DIR/runXX```, which we working around by creating a symlink like ```ln -s /eicdata/eic0005/runXX /eicdata/eic0005/run17/root/runXX```.

Automating the Process
======================

First, you need to follow the link below to set up a tunnel to the blue and yellow pC machines at IP 12. This will provide you with a tunnel into the machines without requiring a password. 

https://wiki.bnl.gov/rhicspin/Automated_pC_setup

The name of the script to execute is `run_asym_daemon.sh`. However, the first line in the script should be changed to the location of the setup.sh and the run number. 

After the tunnel has been established you can start the automated process by executing the following command: 
    
    nohup ./run_asym_daemon.sh > some_output_file_name

and you can put an ampersand at the end of the process if you want to hide it. The ampersand will allow you to continue working in your current terminal. Regardless, the process will be running and looking for new files every 60 seconds. 

Maintaining local copies of GPM logs
====================================

There are currently two interfaces to query Gpm log data (such as bias currents, beam luminosities, ags polarization measurements):

* ```SshLogReader``` (with caching optionally provided by ```CachingLogReader``` wrapper)
* ```opencdev::LocalLogReader```

The first one requires starting a new ssh connection to a machine in the CAD network to run exportLoggerData for each query. This method is generally slow and it is not working reliably with large volumes of parallel queries (which you get when you have multiple instances of asym running). The second method is fast and doesn't require tunnel at runtime, but it needs a full copy of the data available locally (on disk at path defined by ```$SLOW_CONTROL_LOG_DIR``` environment variable). There is a small script (```cdev_export.py```) to maintain such a copy.

Installing dependencies for cdev_export.py
------------------------------------------

To have get it working you will need freetds and python-sybase libraries installed first:

    wget http://mirrors.ibiblio.org/freetds/stable/freetds-0.91.tar.gz
    tar zxf freetds-0.91.tar.gz
    pushd freetds-0.91
    ./configure --prefix=$HOME/local && make && make install
    popd
    wget --no-check-certificate https://sourceforge.net/projects/python-sybase/files/python-sybase/python-sybase-0.40pre2/python-sybase-0.40pre2.tar.gz
    tar zxf python-sybase-0.40pre2.tar.gz
    pushd python-sybase-0.40pre2
    mkdir -p $HOME/local/lib64/python2.6/site-packages/
    SYBASE=$HOME/local CFLAGS="-DHAVE_FREETDS -UWANT_BULKCOPY" PYTHONPATH=$PYTHONPATH:$HOME/local/lib64/python2.6/site-packages python setup.py install --prefix=$HOME/local
    popd
    
You also need to get most recent version of opencdev:

    git clone https://github.com/rhicspin/opencdev.git

Running cdev_export.py
----------------------

The tool needs a connection to the database:

    ssh yellowpc -L 5000:acnlinbd:5000 -Nf
    
Environment variables need to be setup to point to the libraries:

    export SYBASE=$HOME/local
    export PYTHONPATH=$PYTHONPATH:$HOME/local/lib64/python2.6/site-packages
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/local/lib
    
The script is ran as:

    opencdev/tools/cdev_export.py

It should iteratively download Gpm index database from remote Sybase server and save the contents into ```./sql``` directory. The sql files should be rendered to local Sqlite database using ```find sql -exec cat {} \; | sqlite db.sqlite``` command. Then raw SDDS files with the data itself are downloaded to ```./run_fyXX``` directory. Both ```db.sqlite``` and data files need to go into the directory pointed by ```$SLOW_CONTROL_LOG_DIR```.

Alpha Calibration File
======================

During the course of the run, a long alpha measurment should occur and create the alpha calibration files needed for properly processing the measurments. Once this file is created you will need to edit the following code to include the file name of the alpha calibration. You can find the code at the following: 

    /contrib/RunPeriod.inc
    
Make sure to change the start_time, the polarimeter_id, the alpha_calib_run_name, and possibly the gain_slope (may not need to include the gain_slope at all). The other parameters are the default tdc and adc cuts on the bananas plots.


General remarks
===============

The code for the analysis of the RHIC polarimeters data has changed
significantly starting 2010. Nevertheless, one may still find it useful to look
through the older documentation which can be found at

http://www4.rcf.bnl.gov/~cnipol/Analysis/pC/index.html

Other polarimetry software packages
-----------------------------------

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
			<p>Supported by the RHIC Spin group and provides the official polarization numbers.</p>
		</td>
	</tr>
	<tr>
		<th>new hjet analysis</th>
		<td>
			<p>An analysis by Andrei Poblaguev that pioneered support of the new JLAB fADC250 VME DAQ system.</p>
		</td>
	</tr>
</table>

<!--
# vim: set noexpandtab:
-->
