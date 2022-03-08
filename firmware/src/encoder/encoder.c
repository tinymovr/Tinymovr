
#include <src/common.h>
#include <src/encoder/ma7xx.h>
#include <src/encoder/hall.h>
#include <src/encoder/encoder.h>

static EncoderConfig config = { 0 };
static EncoderState state = { 0 };

void encoder_init(void)
{
    if (ENCODER_MA7XX == config.encoder_type)
    {
        ma7xx_init();
        state.current_encoder_type = ENCODER_MA7XX;
        state.get_angle_ptr = &ma7xx_get_angle_rectified;
        state.update_angle_ptr = ma7xx_update_angle;
    }
    else if (ENCODER_HALL == config.encoder_type)
    {
        hall_init();
        state.current_encoder_type = ENCODER_HALL;
        state.get_angle_ptr = &hall_get_angle;
    }
}

PAC5XXX_RAMFUNC int16_t encoder_get_angle(void)
{
    return state.get_angle_ptr();
}

PAC5XXX_RAMFUNC void encoder_update_angle(bool check_error)
{
    if (state.update_angle_ptr)
    {
        state.update_angle_ptr(check_error);
    }
}

PAC5XXX_RAMFUNC uint16_t encoder_get_half_ticks(void)
{
    return ENCODER_MA7XX == state.current_encoder_type ? ENCODER_HALF_TICKS : HALL_HALF_SECTORS;
}

EncoderType encoder_get_type(void)
{
    return state.current_encoder_type;
}

PAC5XXX_RAMFUNC void encoder_set_type(EncoderType enc_type)
{
    if (ENCODER_MA7XX == enc_type)
    {
        config.encoder_type = ENCODER_MA7XX;
    }
    else if (ENCODER_HALL == enc_type)
    {
        config.encoder_type = ENCODER_HALL;
    }
}

EncoderConfig* encoder_get_config(void)
{
    return &config;
}

void encoder_restore_config(EncoderConfig* config_)
{
    config = *config_;
}