
#define HFI_SAMPLE_COUNT 8
#define HFI_SAMPLE_STEP (32/HFI_SAMPLE_COUNT)
#define HFI_VOLTAGE 1.0

typedef struct {
    float I_diffs[HFI_SAMPLE_COUNT];
    uint8_t current_step;
    uint8_t substep;
    float angle_comp;
    float I_prev;
} HFIState;

void hfi_update(void);