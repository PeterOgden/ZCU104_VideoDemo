############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project filter_pipeline
set_top filter_pipeline
add_files filter_pipeline/filter_pipeline.cpp
add_files filter_pipeline/filter_pipeline.h
add_files -tb filter_pipeline/filter_pipeline_test.cpp
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-i} -tool vivado
create_clock -period 3.3 -name default
#source "./filter_pipeline/solution1/directives.tcl"
csim_design
csynth_design
cosim_design -trace_level all
export_design -rtl verilog -format ip_catalog
exit
