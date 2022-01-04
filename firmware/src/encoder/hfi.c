

#include <src/adc/adc.h>
#include <src/encoder/hfi.h>

static HFIState state = {0};

void hfi_update(void)
{
	uint8_t next_sign = 1;
	const uint8_t lut_index = state.current_step * HFI_SAMPLE_STEP;
	const float I_meas_a = state.I_phase_meas.A;
    const float I_meas_b = one_by_sqrt3 * (state.I_phase_meas.B - state.I_phase_meas.C);

    if (1 == state.substep) // even
    { 
		state.I_diffs[state.current_step] = 
			(I_meas_a * utils_tab_cos_32_1[lut_index] + I_meas_b * utils_tab_sin_32_1[lut_index]) - state.I_prev;
		next_sign = -1;
		state.substep = 0;
		state.current_step++;
    }
    else // odd
    {
		state.I_prev = I_meas_a * utils_tab_cos_32_1[lut_index] + I_meas_b * utils_tab_sin_32_1[lut_index];
		state.substep = 1;		
    }

	const float mod_val = HFI_VOLTAGE / ADC_GetVBus();
	const float mod_a = next_sign * mod_val * utils_tab_cos_32_1[lut_index];
	const float mod_b = next_sign * mod_val * utils_tab_sin_32_1[lut_index];
	SVM(mod_a, mod_b, &state.modulation_values.A,
		&state.modulation_values.B, &state.modulation_values.C);

    if (state.current_step == HFI_SAMPLE_COUNT) {
        float c_real, c_im;
        fft_8_bin1(state.I_diffs, c_real, c_im);

        const float f_mag = sqrtf(c_real * c_real + c_im * c_im);
        const float f_phase = atan2f(c_im, c_real);

		// compute & store angle

        state.current_step = 0;
    }
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