
#define HFI_SAMPLE_COUNT 8

typedef struct {
    float I_diffs[HFI_SAMPLE_COUNT];
    uint8_t current_step;
    float angle_comp;
} HFIState;

void hfi_update(void);