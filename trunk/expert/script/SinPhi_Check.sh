#!/bin/bash

mkDB.sh --exclusive -f .cnipol_daemon_run.list | tee dat/asym_analysis_summary.dat

ExeHID36230.sh | tee dat/Total_HID36230.dat

sinphifit.sh 2>/dev/null | tee dat/SinPhi_Check.dat



