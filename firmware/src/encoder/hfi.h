
#define HFI_SAMPLE_COUNT 16
#define HFI_SAMPLE_STRIDE (32/HFI_SAMPLE_COUNT)
#define HFI_VOLTAGE 1.0f

typedef struct {
    float I_diffs[HFI_SAMPLE_COUNT];
    uint8_t current_step;
    uint8_t substep;
    float angle;
    float phase;
    float magnitude;
    float I_prev;
} HFIState;

void hfi_update(void);