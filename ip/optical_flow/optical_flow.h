#pragma once

#include <ap_int.h>
typedef ap_uint<16> InPixel;

typedef ap_uint<32> OutPixel;

typedef ap_fixed<16, 8, AP_RND, AP_SAT> coeff_type;
struct coeffs_matrix {
	coeff_type yr;
	coeff_type yg;
	coeff_type yb;
	coeff_type xfr;
	coeff_type xfg;
	coeff_type xfb;
	coeff_type yfr;
	coeff_type yfg;
	coeff_type yfb;
	coeff_type cr;
	coeff_type cg;
	coeff_type cb;
};

void optical_flow(InPixel* curr_frame, InPixel* prev_frame, OutPixel* out_frame,
		coeffs_matrix coeffs, int rows, int cols);
