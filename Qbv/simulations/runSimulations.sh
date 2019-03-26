#!/bin/bash

case "$1" in
"gen_csv")
	scavetool s -p "(name(time_deviation_histogram:*) AND module(Qbv.gPtpSlave0.eth[0].etherGPTP))" -O results/asymmetric_test.csv -F csv results/asymmetric_test*.sca
    ;;
"gen_plot")
	echo "generating plot"
	python gen_plot.py results/asymmetric_test.csv
	;;
*)
    echo "Unknown command. Try './$0 run'"
    ;;
esac
