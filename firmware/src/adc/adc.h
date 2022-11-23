
//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2020 Ioannis Chatzikonstantinou.
//  *
//  * This program is free software: you can redistribute it and/or modify
//  * it under the terms of the GNU General Public License as published by
//  * the Free Software Foundation, version 3.
//  *
//  * This program is distributed in the hope that it will be useful, but
//  * WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//  * General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License
//  * along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef ADC_ADC_H_
#define ADC_ADC_H_

#include <src/common.h>

#ifdef CAFE_ARCH2
typedef enum
{
#if defined(PAC5527)
    PWRCTL_PWRMON_VCORE0 = 0u << 3,      // 0<<3 --> VCORE
    PWRCTL_PWRMON_VP = 1u << 3,          // 1<<3 --> VP * 1/10
    PWRCTL_PWRMON_VCC33 = 2u << 3,       // 2<<3 --> VCC33 * 4/10
    PWRCTL_PWRMON_VCCIO = 3u << 3,       // 3<<3 --> VCCIO * 4/10
    PWRCTL_PWRMON_VSYS = 4u << 3,        // 4<<3 --> VSYS * 4/10
    PWRCTL_PWRMON_VREFDIV2 = 5u << 3,    // 5<<3 --> VREF/2
    PWRCTL_PWRMON_VPTAT = 6u << 3,       // 6<<3 --> VPTAT
    PWRCTL_PWRMON_VCP_MINUS_VM = 7u << 3 // 7<<3 --> (VCP-VM) * 5/10
#endif                                   // #if defined(PAC5527)
} ADC_PWRCTL_PWRMON_Type;
#endif // #ifdef CAFE_ARCH2

#ifdef CAFE_ARCH2
// MODULE MISC CAFE REGISTER
#define HIB_BIT_DEFAULT 0u      // 0 = Normal; 1 = Hibernate
#define PBEN_BIT_DEFAULT 0u     // 0 = Push Button Disabled; 1 = Push Button Enabled (AIO6)
#define VREFSET_BIT_DEFAULT 0u  // 0 = ADC VREF is 2.5V; 1 = ADC VREF is 3.0V
#define CLKOUTEN_BIT_DEFAULT 0u // 0 = Disabled; 1 = Enabled
#define MCUALIVE_BIT_DEFAULT 1u // 0 = Disabled; 1 = Enabled
#define TPBD_BIT_DEFAULT 1u     // 0 = Disabled; 1 = Enabled
#define RFU_BIT_DEFAULT 0u      // 0 = Disabled; 1 = Enabled
#define ENSIG_BIT_DEFAULT 1u    // 0 = Disabled; 1 = Enabled
#define MODULE_MISC_BITS_DEFAULT ((HIB_BIT_DEFAULT << 7) + (PBEN_BIT_DEFAULT << 6) + (VREFSET_BIT_DEFAULT << 5) + (CLKOUTEN_BIT_DEFAULT << 4) + (MCUALIVE_BIT_DEFAULT << 3) + (TPBD_BIT_DEFAULT << 2) + (RFU_BIT_DEFAULT << 1) + (ENSIG_BIT_DEFAULT << 0))
#endif

#define GAINx1                          (0x01 << 3)
#define GAINx2                          (0x02 << 3)
#define GAINx4                          (0x03 << 3)
#define GAINx8                          (0x04 << 3)
#define GAINx16                         (0x05 << 3)
#define GAINx32                         (0x06 << 3)
#define GAINx64                         (0x07 << 3)

// Current sensing multipler defined as follows:
// meas * ONE_OVER_ADC_RES * VREF / R * ONE_OVER_ADC_GAIN

// Tinymovr R3.3 (alpha2) and later have 2mOhm shunts, ADC gain x16
// 1/2^12 * 3.3 / 0.002 / 16 ~= 0.025177
#if defined BOARD_REV_R33 || defined BOARD_REV_R5
#define SHUNT_SCALING_FACTOR ( 0.025177f )
#define ADC_GAIN_VAL GAINx16

// Tinymovr R3.2 (alpha1) and earlier have 1mOhm shunts, ADC gain x16
// 1/2^12 * 3.3 / 0.001 / 16 ~= 0.050354
#elif defined BOARD_REV_R32
#define SHUNT_SCALING_FACTOR (0.0503f)
#define ADC_GAIN_VAL GAINx16

// Tinymovr M5 have 50mOhm shunts, ADC gain x2
// 1/2^12 * 3.3 / 0.05 / 2 ~= 0.008056
#elif defined BOARD_REV_M5
#define SHUNT_SCALING_FACTOR (0.008056f)
#define ADC_GAIN_VAL GAINx2

#endif

// Inverse of sensing multiplier
#define ONE_OVER_SHUNT_SCALING_FACTOR (1.0f / SHUNT_SCALING_FACTOR)

// VBus scaling factor
#define VBUS_SCALING_FACTOR (0.0128f)

typedef struct 
{
    int16_t temp;
    struct FloatTriplet I_phase_meas;
    struct FloatTriplet I_phase_offset;
} ADCState;

typedef struct
{
    float Iphase_limit;
    float I_filter_k;
    float I_phase_offset_tau;
} ADCConfig;

void ADC_Init(void);
int16_t adc_get_mcu_temp(void);
void ADC_GetPhaseCurrents(struct FloatTriplet *phc);
void ADC_update(void);

#endif /* ADC_ADC_H_ */
