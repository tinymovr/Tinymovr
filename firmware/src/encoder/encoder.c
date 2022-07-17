
#include <src/common.h>
#include <src/motor/motor.h>
#include <src/encoder/ma7xx.h>
#include <src/encoder/hall.h>
#include <src/encoder/encoder.h>

static EncoderConfig config = {0};
static EncoderState state = {0};

void encoder_init(void)
{
#ifdef BOARD_REV_R5
    if (ENCODER_MA7XX == config.encoder_type)
    {
#endif
        ma7xx_init();
        state.current_encoder_type = ENCODER_MA7XX;
        state.get_error_ptr = &ma7xx_get_errors;
        state.get_calibrated_ptr = &ma7xx_rec_is_calibrated;
        state.get_angle_ptr = &ma7xx_get_angle_rectified;
        state.update_angle_ptr = &ma7xx_update;
        state.reset_encoder_ptr = &ma7xx_clear_rec_table;
        state.ticks = ENCODER_TICKS;
#ifdef BOARD_REV_R5
    }
    else if (ENCODER_HALL == config.encoder_type)
    {
        hall_init();
        state.current_encoder_type = ENCODER_HALL;
        state.get_error_ptr = &hall_get_errors;
        state.get_calibrated_ptr = &hall_sector_map_is_calibrated;
        state.get_angle_ptr = &hall_get_angle;
        state.update_angle_ptr = &hall_update;
        state.reset_encoder_ptr = &hall_clear_sector_map;
        state.ticks = HALL_SECTORS;
    }
#endif
}

void encoder_reset(void)
{
    state.reset_encoder_ptr();
}

PAC5XXX_RAMFUNC int16_t encoder_get_angle(void)
{
    return state.get_angle_ptr();
}

PAC5XXX_RAMFUNC void encoder_update(bool check_error)
{
    if (state.update_angle_ptr)
    {
        state.update_angle_ptr(check_error);
    }
}

PAC5XXX_RAMFUNC uint16_t encoder_get_ticks(void)
{
    return state.ticks;
}

PAC5XXX_RAMFUNC float encoder_ticks_to_eangle()
{
#ifdef BOARD_REV_R5
    // We need to derive this during call, because the motor pole pairs
    // may change after calibration, or after user input
    if (ENCODER_MA7XX == state.current_encoder_type)
    {
#endif
        return twopi_by_enc_ticks * motor_get_pole_pairs();
#ifdef BOARD_REV_R5
    }
    return  twopi_by_hall_sectors;
#endif
}

EncoderType encoder_get_type(void)
{
    return state.current_encoder_type;
}

PAC5XXX_RAMFUNC bool encoder_get_calibrated(void)
{
    return state.get_calibrated_ptr();
}

PAC5XXX_RAMFUNC void encoder_set_type(EncoderType enc_type)
{
#ifdef BOARD_REV_R5
    if (ENCODER_MA7XX == enc_type)
    {
#endif
        config.encoder_type = ENCODER_MA7XX;
#ifdef BOARD_REV_R5
    }
    else if (ENCODER_HALL == enc_type)
    {
        config.encoder_type = ENCODER_HALL;
    }
#endif
}

PAC5XXX_RAMFUNC uint8_t encoder_get_errors(void)
{
    return state.get_error_ptr();
}

EncoderConfig* encoder_get_config(void)
{
    return &config;
}

void encoder_restore_config(EncoderConfig* config_)
{
    config = *config_;
}
