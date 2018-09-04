############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project optical_flow
set_top optical_flow
add_files optical_flow/optical_flow.cpp -cflags "-I/home/ogden/git/xfopencv/include"
add_files optical_flow/optical_flow.h
add_files -tb optical_flow/optical_flow_test.cpp
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-i} -tool vivado
create_clock -period 3.33 -name default
#source "./optical_flow/solution1/directives.tcl"
csim_design
csynth_design
cosim_design -trace_level all
export_design -rtl verilog -format ip_catalog
exit
