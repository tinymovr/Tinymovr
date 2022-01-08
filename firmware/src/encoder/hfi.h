
#define HFI_VOLTAGE 0.005f
#define HFI_UPDATE_GAIN 0.01f

typedef struct {
    float angle;
    float phase;
    float magnitude;
    float c_real;
    float c_imag;
    uint8_t current_idx;
    uint8_t substep;
    float I_prev;
    float c_reals[32];
    float c_imags[32];
} HFIState;

void hfi_update(void);

float hfi_get_mag(void);
float hfi_get_phase(void);