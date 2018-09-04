#include <hls_stream.h>
#include <ap_int.h>

struct video_stream_element {
	ap_uint<64> data;
	ap_uint<1> last;
	ap_uint<1> user;
};

typedef hls::stream<video_stream_element> video_stream;

void logo_output(ap_uint<16> width, ap_uint<16> height, video_stream& out_stream,
		unsigned int data[256 * 256]);

unsigned int data [256 * 256];

int main() {
	video_stream out_stream;
	for (int i = 0; i < 256 * 256; ++i) {
		data[i] = i;
	}
	logo_output(128, 256, out_stream, data);
	int counter = 0;
	for (int i = 0; i < 256; ++i) {
		for (int j = 0; j < 128; ++j) {
			video_stream_element e = out_stream.read();
			assert(e.user == ((i == 0) && (j == 0)));
			assert(e.last == (j == 127));
			assert(e.data.range(31,0) == counter++);
			assert(e.data.range(63,32) == counter++);
		}
	}
}
