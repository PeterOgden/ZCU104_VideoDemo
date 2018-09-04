#include <imgproc/xf_dense_npyr_optical_flow.hpp>
#include "optical_flow.h"

#define MAX_ROWS 1080
#define MAX_COLS 1920

void extract_y(InPixel* frame, ap_uint<8>* data, int rows, int cols) {
	assert(rows <= MAX_ROWS);
	assert(cols <= MAX_COLS);
	assert(rows > 0);
	assert(cols > 0);
	for (int i = 0; i < rows; ++i) {
#pragma HLS LOOP_TRIPCOUNT max=1080
		for (int j = 0; j < cols; ++j) {
#pragma HLS LOOP_FLATTEN off
#pragma HLS LOOP_TRIPCOUNT max=1920
#pragma HLS PIPELINE II=1
			*data++ = (*frame++) >> 8;
		}
	}
}

void format_output(ap_uint<8>* frame, float* xflow, float* yflow,
		OutPixel* outdata, coeffs_matrix c, int rows, int cols) {
	assert(rows <= MAX_ROWS);
	assert(cols <= MAX_COLS);
	assert(rows > 0);
	assert(cols > 0);
	for (int i = 0; i < rows; ++i) {
#pragma HLS LOOP_TRIPCOUNT max=1080
		for (int j = 0; j < cols; ++j) {
#pragma HLS LOOP_FLATTEN off
#pragma HLS LOOP_TRIPCOUNT max=1920
#pragma HLS PIPELINE II=1
			OutPixel output;
			coeff_type y, xf, yf;
			y = 0;
			y.range(7,0) = *frame++;
			xf = *xflow++;
			yf = *yflow++;
			coeff_type out_r = y * c.yr + xf * c.xfr + yf * c.yfr + c.cr;
			coeff_type out_b = y * c.yb + xf * c.xfb + yf * c.yfb + c.cb;
			coeff_type out_g = y * c.yg + xf * c.xfg + yf * c.yfg + c.cg;
			ap_int<16> ir, ib, ig;
			ir = out_r.range();
			ig = out_g.range();
			ib = out_b.range();
			if (ir > 255) ir = 255;
			if (ir < 0) ir = 0;
			if (ig > 255) ig = 255;
			if (ig < 0) ig = 0;
			if (ib > 255) ib = 255;
			if (ib < 0) ib = 0;
			output.range(7,0) = ir.range(7,0);
			output.range(15,8) = ig.range(7,0);
			output.range(23,16) = ib.range(7,0);
			output.range(31,24) = 0;
			*outdata++ = output;
		}
	}
}

template <typename T>
void duplicate(T* source, T* dest1, T* dest2, int rows, int cols) {
	assert(rows <= MAX_ROWS);
	assert(cols <= MAX_COLS);
	assert(rows > 0);
	assert(cols > 0);
	for (int i = 0; i < rows; ++i) {
#pragma HLS LOOP_TRIPCOUNT max=1080
		for (int j = 0; j < cols; ++j) {
#pragma HLS LOOP_FLATTEN off
#pragma HLS LOOP_TRIPCOUNT max=1920
			T t = *source++;
			*dest1++ = t;
			*dest2++ = t;
		}
	}
}


ap_uint<8> raw_data[MAX_ROWS * MAX_COLS];
ap_uint<8> buffered_data[MAX_ROWS * MAX_COLS];
ap_uint<8> curr_data[MAX_ROWS * MAX_COLS];
ap_uint<8> prev_data[MAX_ROWS * MAX_COLS];

float xflow[MAX_ROWS * MAX_COLS];
float yflow[MAX_ROWS * MAX_COLS];

void optical_flow(InPixel* curr_frame, InPixel* prev_frame, OutPixel* out_frame,
		coeffs_matrix coeffs, int rows, int cols) {
#pragma HLS INTERFACE s_axilite port=rows
#pragma HLS INTERFACE s_axilite port=cols
#pragma HLS INTERFACE s_axilite port=coeffs
#pragma HLS INTERFACE s_axilite port=return
#pragma HLS INTERFACE m_axi depth=2073600 latency=200 port=curr_frame offset=slave bundle=curr_frame num_read_outstanding=8 num_write_outstanding=8 max_read_burst_length=256 max_write_burst_length=256
#pragma HLS INTERFACE m_axi depth=2073600 latency=200 port=prev_frame offset=slave bundle=prev_frame num_read_outstanding=8 num_write_outstanding=8 max_read_burst_length=256 max_write_burst_length=256
#pragma HLS INTERFACE m_axi depth=2073600 latency=200 port=out_frame offset=slave bundle=out_frame num_read_outstanding=8 num_write_outstanding=8 max_read_burst_length=256 max_write_burst_length=256

	assert(rows <= MAX_ROWS);
	assert(cols <= MAX_COLS);
	assert(rows > 0);
	assert(cols > 0);

#pragma HLS DATAFLOW
#pragma HLS STREAM variable=raw_data dim=1 depth=16
#pragma HLS STREAM variable=buffered_data dim=1 depth=100000
#pragma HLS STREAM variable=curr_data dim=1 depth=16
#pragma HLS STREAM variable=prev_data dim=1 depth=16

	extract_y(curr_frame, raw_data, rows, cols);
	duplicate(raw_data, curr_data, buffered_data, rows, cols);
	extract_y(prev_frame, prev_data, rows, cols);
#pragma HLS STREAM variable=xflow dim=1 depth=16
#pragma HLS STREAM variable=yflow dim=1 depth=16
	xf::fpga_optflow8 <MAX_ROWS, MAX_COLS, 1, 25>(curr_data, prev_data, xflow, yflow, rows, cols, rows * cols);

	format_output(buffered_data, xflow, yflow, out_frame, coeffs, rows, cols);
}
