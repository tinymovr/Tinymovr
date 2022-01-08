
#include <math.h>
#include <src/adc/adc.h>
#include <src/controller/controller.h>
#include <src/utils/utils.h>
#include <src/encoder/hfi_helpers.h>
#include <src/encoder/hfi.h>

static HFIState state = {0};

static unsigned long x=123456789, y=362436069, z=521288629;

unsigned long xorshf96(void) {          //period 2^96-1
    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

   t = x;
   x = y;
   y = z;
   z = t ^ x ^ y;

  return z;
}

void hfi_update(void)
{
    const uint8_t lut_index = state.current_idx;
    uint8_t next_sign = 1;
    FloatTriplet I_phase_meas = {0.0f};
    ADC_get_I_phase(&I_phase_meas);
    const float I_meas_a = I_phase_meas.A;
    const float I_meas_b = one_by_sqrt3 * (I_phase_meas.B - I_phase_meas.C);
    if (1 == state.substep)
    {
        const float I_diff = (I_meas_a * utils_tab_cos_32_2[lut_index] + I_meas_b * utils_tab_sin_32_2[lut_index]) - state.I_prev;
        const float diff_real_conv = I_diff * utils_tab_cos_32_2[lut_index] - state.c_reals[lut_index];
        const float diff_imag_conv = -I_diff * utils_tab_sin_32_2[lut_index] - state.c_imags[lut_index];
        state.c_reals[lut_index] += diff_real_conv * HFI_UPDATE_GAIN;
        state.c_imags[lut_index] += diff_imag_conv * HFI_UPDATE_GAIN;
        state.c_real += diff_real_conv * HFI_UPDATE_GAIN;
        state.c_imag += diff_imag_conv * HFI_UPDATE_GAIN;
        const float c_real = state.c_real / 32.0f;
        const float c_imag = state.c_imag / 32.0f;
        state.magnitude = fast_sqrt(c_real * c_real + c_imag * c_imag);
        state.phase = atan2f(c_imag, c_real);

        next_sign = -1;
        state.substep = 0;
        state.current_idx = xorshf96() % 32;
    }
    else
    {
        state.I_prev = (I_meas_a * utils_tab_cos_32_2[lut_index] + I_meas_b * utils_tab_sin_32_2[lut_index]);
        state.substep = 1;
    }
    const float mod_val = HFI_VOLTAGE / ADC_GetVBus();
    const float mod_a = next_sign * mod_val * utils_tab_cos_32_2[state.current_idx];
    const float mod_b = next_sign * mod_val * utils_tab_sin_32_2[state.current_idx];
    controller_adjust_ab_values(mod_a, mod_b);

// 	uint8_t next_sign = 1;
// 	const uint8_t lut_index = state.current_step * HFI_SAMPLE_STRIDE;
// 	FloatTriplet I_phase_meas = {0.0f};
// 	ADC_get_I_phase(&I_phase_meas);
// 	const float I_meas_a = I_phase_meas.A;
//     const float I_meas_b = one_by_sqrt3 * (I_phase_meas.B - I_phase_meas.C);

//     if (1 == state.substep)
//     { 
// 		state.I_diffs[state.current_step] = 
// 			(I_meas_a * utils_tab_cos_32_2[lut_index] + I_meas_b * utils_tab_sin_32_2[lut_index]) - state.I_prev;
// 		next_sign = -1;
// 		state.substep = 0;
// 		state.current_step++;
//     }
//     else
//     {
// 		state.I_prev = I_meas_a * utils_tab_cos_32_2[lut_index] + I_meas_b * utils_tab_sin_32_2[lut_index];
// 		state.substep = 1;		
//     }

// 	const float mod_val = HFI_VOLTAGE / ADC_GetVBus();
// 	const float mod_a = next_sign * mod_val * utils_tab_cos_32_2[lut_index];
// 	const float mod_b = next_sign * mod_val * utils_tab_sin_32_2[lut_index];

// 	controller_adjust_ab_values(mod_a, mod_b);

//     if (HFI_SAMPLE_COUNT == state.current_step) {
//         float c_real, c_im;

// #ifndef HFI_SAMPLE_COUNT
//         #error "HFI_SAMPLE_COUNT undefined"
// #elif HFI_SAMPLE_COUNT == 8
//         utils_fft8_bin2(state.I_diffs, &c_real, &c_im);
// #elif HFI_SAMPLE_COUNT == 16
//         utils_fft16_bin2(state.I_diffs, &c_real, &c_im);
// #elif HFI_SAMPLE_COUNT == 32
//         utils_fft32_bin2(state.I_diffs, &c_real, &c_im);
// #else
//         #error "Unexpected value of HFI_SAMPLE_COUNT"
// #endif

//         const float f_mag = fast_sqrt(c_real * c_real + c_im * c_im);
//         const float f_phase = atan2f(c_im, c_real);

//         state.magnitude = f_mag;
//         state.phase = f_phase;

// 		// compute & store angle

//         state.current_step = 0;
//     }
}

float hfi_get_mag(void)
{
    return state.magnitude;
}

float hfi_get_phase(void)
{
    return state.phase;
}
