#pragma once

#include <ap_int.h>

typedef ap_fixed<16,8, AP_RND, AP_SAT> coeff_t;
typedef ap_uint<8> threshold_t;
typedef ap_uint<1> bool_t;

void filter_pipeline(unsigned int* in_frame, unsigned int* out_frame,
		coeff_t filter_coeffs[5][5], threshold_t min_thresh, threshold_t max_t,
		bool_t colour, bool_t absolute, unsigned int rows, unsigned int cols);
