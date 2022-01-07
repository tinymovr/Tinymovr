
#include <math.h>
#include <src/adc/adc.h>
#include <src/controller/controller.h>
#include <src/utils/utils.h>
#include <src/encoder/hfi_helpers.h>
#include <src/encoder/hfi.h>

static HFIState state = {0};

void hfi_update(void)
{
	uint8_t next_sign = 1;
	const uint8_t lut_index = state.current_step * HFI_SAMPLE_STRIDE;
	FloatTriplet I_phase_meas = {0.0f};
	ADC_get_I_phase(&I_phase_meas);
	const float I_meas_a = I_phase_meas.A;
    const float I_meas_b = one_by_sqrt3 * (I_phase_meas.B - I_phase_meas.C);

    if (1 == state.substep) // even
    { 
		state.I_diffs[state.current_step] = 
			(I_meas_a * utils_tab_cos_32_2[lut_index] + I_meas_b * utils_tab_sin_32_2[lut_index]) - state.I_prev;
		next_sign = -1;
		state.substep = 0;
		state.current_step++;
    }
    else // odd
    {
		state.I_prev = I_meas_a * utils_tab_cos_32_2[lut_index] + I_meas_b * utils_tab_sin_32_2[lut_index];
		state.substep = 1;		
    }

	const float mod_val = HFI_VOLTAGE / ADC_GetVBus();
	const float mod_a = next_sign * mod_val * utils_tab_cos_32_2[lut_index];
	const float mod_b = next_sign * mod_val * utils_tab_sin_32_2[lut_index];

	FloatTriplet mod_values = {0.0f};
	SVM(mod_a, mod_b, &mod_values.A, &mod_values.B, &mod_values.C);
	controller_adjust_modulation_values(&mod_values);

    if (HFI_SAMPLE_COUNT == state.current_step) {
        float c_real, c_im;
        utils_fft8_bin1(state.I_diffs, &c_real, &c_im);

        const float f_mag = fast_sqrt(c_real * c_real + c_im * c_im);
        const float f_phase = atan2f(c_im, c_real);

        state.magnitude = f_mag;
        state.phase = f_phase;

		// compute & store angle

        state.current_step = 0;
    }
}

float hfi_get_mag(void)
{
    return state.magnitude;
}

float hfi_get_phase(void)
{
    return state.phase;
}
