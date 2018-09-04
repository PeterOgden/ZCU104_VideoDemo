#include "filter_pipeline.h"
#include <hls_video.h>

#define ROWS 1080
#define COLS 1920

typedef hls::Mat<ROWS, COLS, HLS_16SC3> filter_mat;
typedef ap_ufixed<8, 0, AP_RND, AP_SAT> pixel_t;

void read_mat(unsigned int* frame, filter_mat& mat, bool colour) {
	for (int i = 0; i < mat.rows; ++i) {
#pragma HLS LOOP_TRIPCOUNT max=1080
		for (int j = 0; j < (mat.cols >> 1); ++j) {
#pragma HLS LOOP_TRIPCOUNT max=960
#pragma HLS LOOP_FLATTEN off
#pragma HLS PIPELINE II=2
			ap_uint<32> pixel = *frame++;
			pixel_t y1, y2, u, v;
			y1.range() = pixel.range(15, 8);
			y2.range() = pixel.range(31, 24);
			u.range() = pixel.range(7, 0);
			v.range() = pixel.range(23, 16);
			if (colour) {
				pixel_t r1, g1, b1, r2, g2, b2;
//				COLOR_OUT_YCBCR = [1, 1.772, 0,
//				                   1, -0.3344136, -0.714136,
//				                   1, 0, 1.402,
//				                   -0.886, 0.529136, -0.701]

				r1 = coeff_t(y1) + coeff_t(1.402)*v - coeff_t(0.701);
				g1 = coeff_t(y1) - coeff_t(0.3344136) * u - coeff_t(0.714136) * v + coeff_t(0.529136);
				b1 = coeff_t(y1) + coeff_t(1.772) * u - coeff_t(0.886);
				r2 = coeff_t(y2) + coeff_t(1.402)*v - coeff_t(0.701);
				g2 = coeff_t(y2) - coeff_t(0.3344136) * u - coeff_t(0.714136) * v + coeff_t(0.529136);
				b2 = coeff_t(y2) + coeff_t(1.772) * u - coeff_t(0.886);
				mat.data_stream[0].write(r1.range());
				mat.data_stream[1].write(g1.range());
				mat.data_stream[2].write(b1.range());
				mat.data_stream[0].write(r2.range());
				mat.data_stream[1].write(g2.range());
				mat.data_stream[2].write(b2.range());
			} else {
				mat.data_stream[0].write(y1.range(7,0));
				mat.data_stream[1].write(y1.range(7,0));
				mat.data_stream[2].write(y1.range(7,0));
				mat.data_stream[0].write(y2.range(7,0));
				mat.data_stream[1].write(y2.range(7,0));
				mat.data_stream[2].write(y2.range(7,0));
			}
		}
	}
}

void write_mat(filter_mat& mat, unsigned int* frame) {
	for (int i = 0; i < mat.rows; ++i) {
#pragma HLS LOOP_TRIPCOUNT max=1080
		for (int j = 0; j < mat.cols; ++j) {
#pragma HLS LOOP_TRIPCOUNT max=1920
#pragma HLS LOOP_FLATTEN off
#pragma HLS PIPELINE II=1
			ap_uint<32> pixel;
			pixel.range(7,0) = mat.data_stream[0].read();
			pixel.range(15,8) = mat.data_stream[1].read();
			pixel.range(23,16) = mat.data_stream[2].read();
			pixel.range(31,24) = 0;
			*frame++ = pixel;
		}
	}
}

void threshold(filter_mat& in, filter_mat& out, bool_t absolute, ap_uint<8> minval, ap_uint<8> maxval) {
	for (int i = 0; i < in.rows; ++i) {
#pragma HLS LOOP_TRIPCOUNT max=1080
		for (int j = 0; j < in.cols; ++j) {
#pragma HLS LOOP_TRIPCOUNT max=1920
#pragma HLS LOOP_FLATTEN off
#pragma HLS PIPELINE II=1
			for (int k = 0; k < 3; ++k) {
#pragma HLS LOOP_UNROLL
				ap_int<16> val = in.data_stream[k].read();
				if (absolute && val < 0) val = -val;
				if (val > maxval) val = 255;
				if (val < minval) val = 0;
				out.data_stream[k].write(val);
			}
		}
	}
}

typedef ap_fixed<24,16,AP_TRN,AP_SAT> filter_type;

void filter(coeff_t coeffs[5][5], filter_mat& in, filter_mat& out) {
#pragma HLS DATAFLOW
        hls::Window<5, 5, filter_type> window;
        for (int i = 0; i < 5; ++i) {
#pragma HLS UNROLL
                for (int j = 0; j < 5; ++j) {
#pragma HLS UNROLL
                        window.val[i][j] = coeffs[i][j];
                }
        }
        hls::Filter2D(in, out, window, hls::Point(2,2));
}

void filter_pipeline(unsigned int* in_frame, unsigned int* out_frame,
		coeff_t filter_coeffs[5][5], threshold_t min_thresh, threshold_t max_thresh,
		bool_t colour, bool_t absolute, unsigned int rows, unsigned int cols) {
#pragma HLS INTERFACE s_axilite port=return
#pragma HLS INTERFACE m_axi depth=1036800 latency=200 port=in_frame offset=slave bundle=frame num_read_outstanding=8 num_write_outstanding=8 max_read_burst_length=256 max_write_burst_length=256
#pragma HLS INTERFACE m_axi depth=2073600 latency=200 port=out_frame offset=slave bundle=frame num_read_outstanding=8 num_write_outstanding=8 max_read_burst_length=256 max_write_burst_length=256
#pragma HLS INTERFACE s_axilite port=filter_coeffs register
#pragma HLS INTERFACE s_axilite port=min_thresh register
#pragma HLS INTERFACE s_axilite port=max_thresh register
#pragma HLS INTERFACE s_axilite port=colour register
#pragma HLS INTERFACE s_axilite port=absolute register
#pragma HLS INTERFACE s_axilite port=rows register
#pragma HLS INTERFACE s_axilite port=cols register

	assert(rows <= ROWS);
	assert(cols <= COLS);
#pragma HLS DATAFLOW
	filter_mat in_mat(rows, cols);
	filter_mat filtered(rows, cols);
	filter_mat out_mat(rows, cols);
	read_mat(in_frame, in_mat, colour);
	filter(filter_coeffs, in_mat, filtered);
	threshold(filtered, out_mat, absolute, min_thresh, max_thresh);
	write_mat(out_mat, out_frame);


}
