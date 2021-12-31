

#include <src/encoder/hfi.h>

void hfi_update(void)
{
    float real_bin1, imag_bin1, real_bin2, imag_bin2;
    motor->m_hfi.fft_bin1_func((float*)motor->m_hfi.buffer, &real_bin1, &imag_bin1);
    motor->m_hfi.fft_bin2_func((float*)motor->m_hfi.buffer, &real_bin2, &imag_bin2);

    float mag_bin_1 = sqrtf(SQ(imag_bin1) + SQ(real_bin1));
    float angle_bin_1 = -utils_fast_atan2(imag_bin1, real_bin1);

    angle_bin_1 += M_PI / 1.7; // Why 1.7??
    utils_norm_angle_rad(&angle_bin_1);

    float mag_bin_2 = sqrtf(SQ(imag_bin2) + SQ(real_bin2));
    float angle_bin_2 = -utils_fast_atan2(imag_bin2, real_bin2) / 2.0;

    motor->m_hfi.angle = angle_bin_2;
	utils_norm_angle_rad((float*)&motor->m_hfi.angle);
}

static void update_hfi_samples(foc_hfi_samples samples, volatile motor_all_state_t *motor) {
	utils_sys_lock_cnt();

	memset((void*)&motor->m_hfi, 0, sizeof(motor->m_hfi));
	switch (samples) {
	case HFI_SAMPLES_8:
		motor->m_hfi.samples = 8;
		motor->m_hfi.table_fact = 4;
		motor->m_hfi.fft_bin0_func = utils_fft8_bin0;
		motor->m_hfi.fft_bin1_func = utils_fft8_bin1;
		motor->m_hfi.fft_bin2_func = utils_fft8_bin2;
		break;

	case HFI_SAMPLES_16:
		motor->m_hfi.samples = 16;
		motor->m_hfi.table_fact = 2;
		motor->m_hfi.fft_bin0_func = utils_fft16_bin0;
		motor->m_hfi.fft_bin1_func = utils_fft16_bin1;
		motor->m_hfi.fft_bin2_func = utils_fft16_bin2;
		break;

	case HFI_SAMPLES_32:
		motor->m_hfi.samples = 32;
		motor->m_hfi.table_fact = 1;
		motor->m_hfi.fft_bin0_func = utils_fft32_bin0;
		motor->m_hfi.fft_bin1_func = utils_fft32_bin1;
		motor->m_hfi.fft_bin2_func = utils_fft32_bin2;
		break;
	}

	utils_sys_unlock_cnt();
}

// Yes, this is only the average...
void utils_fft32_bin0(float *real_in, float *real, float *imag) {
	*real = 0.0;
	*imag = 0.0;

	for (int i = 0;i < 32;i++) {
		*real += real_in[i];
	}

	*real /= 32.0;
}

void utils_fft32_bin1(float *real_in, float *real, float *imag) {
	*real = 0.0;
	*imag = 0.0;
	for (int i = 0;i < 32;i++) {
		*real += real_in[i] * utils_tab_cos_32_1[i];
		*imag -= real_in[i] * utils_tab_sin_32_1[i];
	}
	*real /= 32.0;
	*imag /= 32.0;
}

void utils_fft32_bin2(float *real_in, float *real, float *imag) {
	*real = 0.0;
	*imag = 0.0;
	for (int i = 0;i < 32;i++) {
		*real += real_in[i] * utils_tab_cos_32_2[i];
		*imag -= real_in[i] * utils_tab_sin_32_2[i];
	}
	*real /= 32.0;
	*imag /= 32.0;
}

void utils_fft16_bin0(float *real_in, float *real, float *imag) {
	*real = 0.0;
	*imag = 0.0;

	for (int i = 0;i < 16;i++) {
		*real += real_in[i];
	}

	*real /= 16.0;
}

void utils_fft16_bin1(float *real_in, float *real, float *imag) {
	*real = 0.0;
	*imag = 0.0;
	for (int i = 0;i < 16;i++) {
		*real += real_in[i] * utils_tab_cos_32_1[2 * i];
		*imag -= real_in[i] * utils_tab_sin_32_1[2 * i];
	}
	*real /= 16.0;
	*imag /= 16.0;
}

void utils_fft16_bin2(float *real_in, float *real, float *imag) {
	*real = 0.0;
	*imag = 0.0;
	for (int i = 0;i < 16;i++) {
		*real += real_in[i] * utils_tab_cos_32_2[2 * i];
		*imag -= real_in[i] * utils_tab_sin_32_2[2 * i];
	}
	*real /= 16.0;
	*imag /= 16.0;
}

void utils_fft8_bin0(float *real_in, float *real, float *imag) {
	*real = 0.0;
	*imag = 0.0;

	for (int i = 0;i < 8;i++) {
		*real += real_in[i];
	}

	*real /= 8.0;
}

void utils_fft8_bin1(float *real_in, float *real, float *imag) {
	*real = 0.0;
	*imag = 0.0;
	for (int i = 0;i < 8;i++) {
		*real += real_in[i] * utils_tab_cos_32_1[4 * i];
		*imag -= real_in[i] * utils_tab_sin_32_1[4 * i];
	}
	*real /= 8.0;
	*imag /= 8.0;
}

void utils_fft8_bin2(float *real_in, float *real, float *imag) {
	*real = 0.0;
	*imag = 0.0;
	for (int i = 0;i < 8;i++) {
		*real += real_in[i] * utils_tab_cos_32_2[4 * i];
		*imag -= real_in[i] * utils_tab_sin_32_2[4 * i];
	}
	*real /= 8.0;
	*imag /= 8.0;
}

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