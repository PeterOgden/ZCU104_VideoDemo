#include "filter_pipeline.h"

void setup_data(unsigned int* frame, int rows, int cols) {
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; j += 2) {
			unsigned int y1 = j & 0xFF;
			unsigned int y2 = (j + 1) & 0xFF;
			unsigned int u = i & 0xFF;
			unsigned int v = 0;
			*frame++ = u | (y1 << 8) | (v << 16) | (y2 << 24);
		}
	}
}

void clear_output(unsigned int* frame) {
	for (int i = 0; i < 1080; ++i) {
		for (int j = 0; j < 1920; ++j) {
			*frame++ = 0;
		}
	}
}

template <typename T>
void checkout_output(unsigned int* frame, int rows, int cols, T func) {
	for (int i = 0; i< rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			unsigned int pixel, r, g, b;
			pixel = *frame++;
			r = pixel & 0xFF;
			g = (pixel >> 8) & 0xFF;
			b = (pixel >> 16) & 0xFF;
			if (!func(i, j, r, g, b)) {
				std::cout << std::hex << "Failure at " << i << ", " << j << ": " << pixel << std::endl;
			}
		}
	}
}

bool ident_grey_check(int i, int j, int r, int g, int b) {
	return (r == (j & 0xFF)) && (g == (j & 0xFF)) && (b == (j & 0xFF));
}

bool threshold_check(int i, int j, int r, int g, int b) {
	int expected = j & 0xFF;
	if (expected < 16) expected = 0;
	if (expected > 200) expected = 255;
	return (r == expected) && (g == expected) && (b == expected);
}

bool black_check(int i, int j, int r, int g, int b) {
	return (r == 0) && (g == 0) && (b == 0);
}

bool within1(int a, int b) {
	return abs(a - b) <= 1;
}

bool ident_colour_check(int i, int j, int r, int g, int b) {
	int y = j & 0xFF;
	int u = i & 0xFF;
	int expected_r = y - 0.701 * 256;
	int expected_g = y - 0.3344136 * u + 0.529136 * 256;
	int expected_b = y + 1.772 * u - 0.886 * 256;
	if (expected_r > 255) expected_r = 255;
	if (expected_g > 255) expected_g = 255;
	if (expected_b > 255) expected_b = 255;
	if (expected_r < 0) expected_r = 0;
	if (expected_g < 0) expected_g = 0;
	if (expected_b < 0) expected_b = 0;
	bool result = within1(r, expected_r) && within1(g, expected_g) && within1(b, expected_b);
	if (!result) {
		std::cout << "FAILED: " << i << ", " << j << ": R " << expected_r << "<>" << r << " " <<
				expected_g << "<>" << g << " " << expected_b << "<>" << b << std::endl;
	}
	return result;
}

#define ROWS 25
#define COLS 1920

int main() {
	unsigned int *in_frame = new unsigned int[960 * 1080];
	unsigned int *out_frame = new unsigned int[1920 * 1080];

	coeff_t filter_coeffs[5][5];
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j)  {
			filter_coeffs[i][j] = 0;
		}
	}
	filter_coeffs[2][2] = 1;
	setup_data(in_frame, ROWS, COLS);
	clear_output(out_frame);
	filter_pipeline(in_frame, out_frame, filter_coeffs, 0, 255, 0, 0, ROWS, COLS);
	checkout_output(out_frame, ROWS, COLS, ident_grey_check);

	clear_output(out_frame);
	filter_pipeline(in_frame, out_frame, filter_coeffs, 0, 255, 1, 0, ROWS, COLS);
	checkout_output(out_frame, ROWS, COLS, ident_colour_check);

	clear_output(out_frame);
	filter_pipeline(in_frame, out_frame, filter_coeffs, 16, 200, 0, 0, ROWS, COLS);
	checkout_output(out_frame, ROWS, COLS, threshold_check);

	filter_coeffs[2][2] = -1;
	clear_output(out_frame);
	filter_pipeline(in_frame, out_frame, filter_coeffs, 0, 255, 0, 0, ROWS, COLS);
	checkout_output(out_frame, ROWS, COLS, black_check);

	clear_output(out_frame);
	filter_pipeline(in_frame, out_frame, filter_coeffs, 0, 255, 0, 1, ROWS, COLS);
	checkout_output(out_frame, ROWS, COLS, ident_grey_check);



}
