
#include <src/common.h>

typedef void (*bool_setter)(bool);
typedef int16_t (*int16_getter)(void);

typedef enum {
	ENCODER_MA7XX = 0,
	ENCODER_HALL = 1
} EncoderType;

typedef struct {
    EncoderType current_encoder_type;
    bool_setter update_angle_ptr;
    int16_getter get_angle_ptr;
    uint16_t half_ticks;
} EncoderState;

typedef struct
{
    EncoderType encoder_type;
} EncoderConfig;

void encoder_init(void);
PAC5XXX_RAMFUNC int16_t encoder_get_angle(void);
PAC5XXX_RAMFUNC void encoder_update_angle(bool check_error);

PAC5XXX_RAMFUNC uint16_t encoder_get_half_ticks(void);
PAC5XXX_RAMFUNC float encoder_ticks_to_eangle(void);

PAC5XXX_RAMFUNC EncoderType encoder_get_type(void);
void encoder_set_type(EncoderType enc_type);

EncoderConfig* encoder_get_config(void);
void encoder_restore_config(EncoderConfig* config_);

