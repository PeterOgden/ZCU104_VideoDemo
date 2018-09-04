#include <hls_stream.h>
#include <ap_int.h>
#include <cassert>

struct video_stream_element {
	ap_uint<64> data;
	ap_uint<1> last;
	ap_uint<1> user;
};

typedef hls::stream<video_stream_element> video_stream;

void logo_output(ap_uint<16> width, ap_uint<16> height, video_stream& out_stream,
		unsigned int data[256 * 256]) {
#pragma HLS INTERFACE s_axilite port=return
#pragma HLS INTERFACE s_axilite port=width register
#pragma HLS INTERFACE s_axilite port=height register
#pragma HLS INTERFACE s_axilite port=data
#pragma HLS INTERFACE axis port=out_stream

	assert(width <= 128);
	assert(height <= 256);

	unsigned int *pointer = data;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_FLATTEN off
			video_stream_element e;
			e.data.range(31, 0) = *pointer++;
			e.data.range(63, 32) = *pointer++;
			e.last = j == (width - 1);
			e.user = (i == 0) && (j == 0);
			out_stream.write(e);
		}
	}
}
