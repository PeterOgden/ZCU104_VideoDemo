############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project pixel_pack_2
set_top pixel_pack
add_files pixel_pack_2/pixel_pack.cpp
add_files -tb pixel_pack_2/pixel_pack_test.cpp
open_solution "solution1"
set_part {xczu7ev-ffvc1156-2-i-es2} -tool vivado
create_clock -period 3.3 -name default
#source "./pixel_pack_2/solution1/directives.tcl"
csim_design -compiler gcc
csynth_design
cosim_design -trace_level port
export_design -rtl verilog -format ip_catalog
exit
