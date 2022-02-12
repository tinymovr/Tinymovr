
#include <src/system/system.h>
#include <src/encoder/hall.h>

#define AIO6789_IO_MODE                 0x00
#define AIO_INPUT                       0x00

static HAllState state = { 0 };

void hall_init(void)
{
    pac5xxx_tile_register_write(ADDR_CFGAIO7, AIO6789_IO_MODE | AIO_INPUT);
    pac5xxx_tile_register_write(ADDR_CFGAIO8, AIO6789_IO_MODE | AIO_INPUT);
    pac5xxx_tile_register_write(ADDR_CFGAIO9, AIO6789_IO_MODE | AIO_INPUT);
}

PAC5XXX_RAMFUNC int16_t hall_get_angle(void)
{
    return state.angle;
}

PAC5XXX_RAMFUNC void hall_update_angle(bool check_error)
{
    const uint8_t sector = (pac5xxx_tile_register_read(ADDR_DINSIG1) >> 1) & 0x07;
    state.sector = sector;
    if (check_error)
    {
    	// add_error_flag(ERROR_ENCODER_READING_UNSTABLE);
    }
    //state.angle = angle;
}

PAC5XXX_RAMFUNC uint8_t hall_get_sector(void)
{
    return state.sector;
}