
const float utils_tab_sin_32_1[] = {
	0.000000, 0.195090, 0.382683, 0.555570, 0.707107, 0.831470, 0.923880, 0.980785,
	1.000000, 0.980785, 0.923880, 0.831470, 0.707107, 0.555570, 0.382683, 0.195090,
	0.000000, -0.195090, -0.382683, -0.555570, -0.707107, -0.831470, -0.923880, -0.980785,
	-1.000000, -0.980785, -0.923880, -0.831470, -0.707107, -0.555570, -0.382683, -0.195090};

const float utils_tab_sin_32_2[] = {
	0.000000, 0.382683, 0.707107, 0.923880, 1.000000, 0.923880, 0.707107, 0.382683,
	0.000000, -0.382683, -0.707107, -0.923880, -1.000000, -0.923880, -0.707107, -0.382683,
	-0.000000, 0.382683, 0.707107, 0.923880, 1.000000, 0.923880, 0.707107, 0.382683,
	0.000000, -0.382683, -0.707107, -0.923880, -1.000000, -0.923880, -0.707107, -0.382683};

const float utils_tab_cos_32_1[] = {
	1.000000, 0.980785, 0.923880, 0.831470, 0.707107, 0.555570, 0.382683, 0.195090,
	0.000000, -0.195090, -0.382683, -0.555570, -0.707107, -0.831470, -0.923880, -0.980785,
	-1.000000, -0.980785, -0.923880, -0.831470, -0.707107, -0.555570, -0.382683, -0.195090,
	-0.000000, 0.195090, 0.382683, 0.555570, 0.707107, 0.831470, 0.923880, 0.980785};

const float utils_tab_cos_32_2[] = {
	1.000000, 0.923880, 0.707107, 0.382683, 0.000000, -0.382683, -0.707107, -0.923880,
	-1.000000, -0.923880, -0.707107, -0.382683, -0.000000, 0.382683, 0.707107, 0.923880,
	1.000000, 0.923880, 0.707107, 0.382683, 0.000000, -0.382683, -0.707107, -0.923880,
	-1.000000, -0.923880, -0.707107, -0.382683, -0.000000, 0.382683, 0.707107, 0.923880};

// Yes, this is only the average...
void utils_fft32_bin0(float *real_in, float *real, float *imag) {
	*real = 0.0f;
	*imag = 0.0f;

	for (int i = 0;i < 32;i++) {
		*real += real_in[i];
	}

	*real /= 32.0f;
}

void utils_fft32_bin1(float *real_in, float *real, float *imag) {
	*real = 0.0f;
	*imag = 0.0f;
	for (int i = 0;i < 32;i++) {
		*real += real_in[i] * utils_tab_cos_32_1[i];
		*imag -= real_in[i] * utils_tab_sin_32_1[i];
	}
	*real /= 32.0f;
	*imag /= 32.0f;
}

void utils_fft32_bin2(float *real_in, float *real, float *imag) {
	*real = 0.0f;
	*imag = 0.0f;
	for (int i = 0;i < 32;i++) {
		*real += real_in[i] * utils_tab_cos_32_2[i];
		*imag -= real_in[i] * utils_tab_sin_32_2[i];
	}
	*real /= 32.0f;
	*imag /= 32.0f;
}

void utils_fft16_bin0(float *real_in, float *real, float *imag) {
	*real = 0.0f;
	*imag = 0.0f;

	for (int i = 0;i < 16;i++) {
		*real += real_in[i];
	}

	*real /= 16.0f;
}

void utils_fft16_bin1(float *real_in, float *real, float *imag) {
	*real = 0.0f;
	*imag = 0.0f;
	for (int i = 0;i < 16;i++) {
		*real += real_in[i] * utils_tab_cos_32_1[2 * i];
		*imag -= real_in[i] * utils_tab_sin_32_1[2 * i];
	}
	*real /= 16.0f;
	*imag /= 16.0f;
}

void utils_fft16_bin2(float *real_in, float *real, float *imag) {
	*real = 0.0f;
	*imag = 0.0f;
	for (int i = 0;i < 16;i++) {
		*real += real_in[i] * utils_tab_cos_32_2[2 * i];
		*imag -= real_in[i] * utils_tab_sin_32_2[2 * i];
	}
	*real /= 16.0f;
	*imag /= 16.0f;
}

void utils_fft8_bin0(float *real_in, float *real, float *imag) {
	*real = 0.0f;
	*imag = 0.0f;

	for (int i = 0;i < 8;i++) {
		*real += real_in[i];
	}

	*real /= 8.0f;
}

void utils_fft8_bin1(float *real_in, float *real, float *imag) {
	*real = 0.0f;
	*imag = 0.0f;
	for (int i = 0;i < 8;i++) {
		*real += real_in[i] * utils_tab_cos_32_1[4 * i];
		*imag -= real_in[i] * utils_tab_sin_32_1[4 * i];
	}
	*real /= 8.0f;
	*imag /= 8.0f;
}

void utils_fft8_bin2(float *real_in, float *real, float *imag) {
	*real = 0.0f;
	*imag = 0.0f;
	for (int i = 0;i < 8;i++) {
		*real += real_in[i] * utils_tab_cos_32_2[4 * i];
		*imag -= real_in[i] * utils_tab_sin_32_2[4 * i];
	}
	*real /= 8.0f;
	*imag /= 8.0f;
}