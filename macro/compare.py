#!/usr/bin/env python2

WORK_DIR = "/eicdata/eic0005/run13/root"
WORK_DIR2 = "/eic/u/dsmirnov/eic0005/run13/log"
PHP_NAME = "runconfig.php"

import os
import math

def parse_php(lines):
	"""
	Turns "$rc['foo'][0]['bar'] = 123" lines into {.., 'foo.0.bar': 123,..}
	"""
	result = {}
	for line in lines:
		lr = line.split("=")
		if len(lr) != 2:
			continue
		key = lr[0].strip()[4:-1].replace("'", "").replace("][", ".")
		val = lr[1].strip().rstrip(";")
		if (val.startswith("'") and val.endswith("'")):
			val = val.strip("'")
		elif (val.startswith("\"") and val.endswith("\"")):
			val = val.strip("\"")
		elif key == 'RUNID':
			pass
		elif val.find(".") != -1:
			try:
				val = float(val)
			except ValueError:
				pass
		else:
			try:
				val = int(val)
			except ValueError:
				pass
		result[key] = val;
	return result

def mean(l):
	return sum(l) / float(len(l))

super_files = {}
def open_super(fname):
	global super_files
	if fname not in super_files:
		print fname
		super_files[fname] = open(fname, "w")
	return super_files[fname]

for RUNID in os.listdir(WORK_DIR):
	try:
		id = int(RUNID.split(".")[0])
		if id < 17200:
			continue
	except ValueError:
		pass
	run_config_path = os.path.join(WORK_DIR, RUNID, PHP_NAME)
	if not os.path.exists(run_config_path):
		continue
	fp = open(run_config_path, "r")
	lines = fp.readlines()
	run_log_path = os.path.join(WORK_DIR2, 'an' + RUNID + '.log')
	if not os.path.exists(run_log_path):
		print "missing ", RUNID
		continue
	fp = open(run_log_path)
	fail = False
	MAGIC_LINE = " Average analyzing power from L.Trueman's fit=  "
	for line in fp.xreadlines():
		if line.find("Polarization") != -1:
			line = line.split(":")[-1].strip()
			try:
				(log_pol, log_pol_err) = \
					map(float, line.split("+-"))
			except ValueError:
				fail = True
		if line.startswith(MAGIC_LINE):
			log_a_n = float(line[len(MAGIC_LINE):])
	if fail:
		continue
	fp.close()
	conf = parse_php(lines)
	if len(conf) == 0:
		print "empty ", RUNID
		continue
	if conf['polarization'] >= .9:
		continue
	if conf['polarization'] <= .1:
		continue
	if conf['polarization_error'] >= .4:
		continue
	if conf['polarization_error'] <= 0:
		continue
	cni_pol = conf['polarization']
	cni_pol_err = conf['polarization_error']
	if log_pol < 0:
		continue
	if int(conf['fBeamEnergy']) in [101, 214, 100]:
		continue
	fname = "%i_%s.data" % (conf['fBeamEnergy'], conf['alpha_calib_run_name'].split(".")[1])
	ff = open_super(fname)
	ff.write("%s\t%f\t%f\n" % (RUNID, log_pol/cni_pol, (log_pol_err*cni_pol + cni_pol_err*log_pol)/cni_pol/cni_pol))
	cni_a_n = float(conf["A_N"].split(",")[1][1:-1])
	fname = "%i_%s_a_n.data" % (conf['fBeamEnergy'], conf['alpha_calib_run_name'].split(".")[1])
	ff = open_super(fname)
	ff.write("%s\t%f\n" % (RUNID, log_a_n/cni_a_n))

def get_run_subid(run):
	return run['RUNID'].split(".")[1]

import subprocess
def fit_file(fname):
	fp = open("fit.gnuplot", "w")
	fp.write("c(x) = C; fit c(x) '%s' via C" % fname)
	fp.close()
	sp = subprocess.Popen(["gnuplot", "fit.gnuplot"], stderr=subprocess.PIPE)
	(_, stderr) = sp.communicate()
	lines = filter(lambda l: l.find("+/-") != -1, stderr.split("\n"))
	assert len(lines) == 1
	line = lines[0].split()
	return float(line[2]), float(line[4])

"""
       set size 0.7,0.7
       set origin 0.1,0.1
       set multiplot
       set size 0.4,0.4
       set origin 0.1,0.1
       plot sin(x)
       set size 0.2,0.2
       set origin 0.5,0.5
       plot cos(x)
       unset multiplot
"""

colors = {'yel1': "gold", 'yel2': "goldenrod", 'blu1': "dodgerblue", 'blu2': "darkslateblue"}

def plot(fname, left, top, bars = True):
	ps.write("set origin %f, %f;\n" % (left, t))
	fp = super_files[fname]
	fp.close()
	(c, cerr) = fit_file(fname)
	ps.write("set label '%s, c = %f +/- %f' at graph 0.5,0.9 center font 'Courier,20'\n" % (fname.split(".")[0], c, cerr))
	if bars:
		bars_str = " with yerrorbars"
	else:
		bars_str = ""
	ps.write("plot '%s' %s, %f\n" % (fname, bars_str, c));
	ps.write("unset label\n")
ps = open("plot.gnuplot", "w")
ps.write("set term gif size 1500,1000\n")
ps.write("set output 'onlinepol_over_offlinepol_results.gif'\n")
ps.write("set key off\n")
ps.write("set grid\n")
margin = 0.8
ps.write("set tmargin %f\n" % margin)
ps.write("set bmargin %f\n" % margin)
ps.write("set lmargin %f\n" % margin)
ps.write("set rmargin %f\n" % margin)
ps.write("set multiplot\n")
ps.write("set size 0.45, 0.17;\n")
t = 0.05
for col, gp_color in colors.items():
	fname = "23_%s.data" % col
	plot(fname, 0.05, t)
        t += 0.25
ps.write("set size 0.40, 0.17;\n")
t = 0.05
for col, gp_color in colors.items():
	fname = "254_%s.data" % col
	plot(fname, 0.58, t)
	t += 0.25

ps.write("unset multiplot;\n")
ps.write("set term gif size 1500,1000\n")
ps.write("set output 'onlinepol_over_offlinepol_a_n.gif'\n")
ps.write("set size;\n")
ps.write("set multiplot\n")
ps.write("set size 0.45, 0.17;\n")
t = 0.05
for col, gp_color in colors.items():
        fname = "23_%s_a_n.data" % col
        plot(fname, 0.05, t, False)
        t += 0.25
ps.write("set size 0.40, 0.17;\n")
t = 0.05 
for col, gp_color in colors.items():
        fname = "254_%s_a_n.data" % col
        plot(fname, 0.58, t, False)
        t += 0.25

ps.write("unset multiplot;\n")

ps.close()
os.system("gnuplot plot.gnuplot")
