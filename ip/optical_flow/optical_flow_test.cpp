#include "optical_flow.h"

InPixel* inframe0;
InPixel* inframe1;
OutPixel* outframe;

void zero_coeffs(coeffs_matrix& c) {
	c.yr = 0;
	c.yg = 0;
	c.yb = 0;
	c.xfr = 0;
	c.xfg = 0;
	c.xfb = 0;
	c.yfr = 0;
	c.yfg = 0;
	c.yfb = 0;
	c.cb = 0;
	c.cr = 0;
	c.cg = 0;
}

void passthrough_test() {
	for (int i = 0; i < 40; ++i) {
		for (int j = 0; j < 1920; ++j) {
			inframe0[i * 1920 + j] = ((i + j) % 256) << 8;
		}
	}
	coeffs_matrix coeffs;
	zero_coeffs(coeffs);
	coeffs.yr = 1;

	optical_flow(inframe0, inframe1, outframe, coeffs, 40, 1920);
	for (int i = 0; i < 40; ++i) {
		for (int j = 0; j < 1920; ++j) {
			InPixel in_p = inframe0[i*1920 + j] >> 8;
			OutPixel out_p = outframe[i*1920 + j] & 0xFF;
			if (in_p != out_p) {
				std::cout << i << ", " << j << ", " << in_p << ", " << out_p << std::endl;
			}
		}
	}
}

void horizontal_shift_test() {
	for (int i = 0; i < 1080; ++i) {
		for (int j = 0; j < 1920; ++j) {
			inframe0[i * 1920 + j] = (i & 0x1) << 3;
			inframe1[i * 1920 + j] = ((i + 5) & 0x1) << 3;
		}
	}
	coeffs_matrix coeffs;
	zero_coeffs(coeffs);
	coeffs.xfr = 4;
	coeffs.yfg = 4;
	coeffs.cr = 0.5;
	coeffs.cg = 0.5;
	optical_flow(inframe0, inframe1, outframe, coeffs, 1080, 1920);
	for (int i = 0; i < 1080; ++i) {
		for (int j = 0; j < 1920; ++j) {
			unsigned int p = outframe[i * 1920 + j];
			unsigned int xf = p & 0xFF;
			unsigned int xy = (p >> 8) & 0xFF;
			if (xf != 128 || xy != 128) {
				std::cout << i << ", " << j << ", " << xf << ", " << xy << std::endl;
			}
		}
	}
}
int main() {
	inframe0 = new InPixel[1920 * 1080];
	inframe1 = new InPixel[1920 * 1080];
	outframe = new OutPixel[1920 * 1080];
	passthrough_test();
	horizontal_shift_test();
}
