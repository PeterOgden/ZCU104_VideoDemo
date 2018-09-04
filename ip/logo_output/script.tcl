############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project logo_output
set_top logo_output
add_files logo_output/logo_output.cpp
add_files -tb logo_output/logo_output_test.cpp
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-i} -tool vivado
create_clock -period 3.3 -name default
source "./logo_output/solution1/directives.tcl"
csim_design
csynth_design
cosim_design -trace_level port
export_design -rtl verilog -format ip_catalog
exit
