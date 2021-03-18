
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

#ifndef GATEDRIVER_GATEDRIVER_H_
#define GATEDRIVER_GATEDRIVER_H_

typedef enum {
	GATEDRIVER_DISABLED = 0,
	GATEDRIVER_ENABLED = 1
} GateDriverState;

struct GateDriver_
{
    GateDriverState state;
};

void GateDriver_Init(void);
PAC5XXX_RAMFUNC void GateDriver_Enable(void);
PAC5XXX_RAMFUNC void GateDriver_Disable(void);
PAC5XXX_RAMFUNC void GateDriver_SetDutyCycle(struct FloatTriplet *dc);


//=============================================
// Motor Driver Duty Cycle Macro Functions
//=============================================
PAC5XXX_RAMFUNC static inline void m1_u_set_duty(float duty)
{
    uint16_t val = ((uint16_t)(duty * (timer_freq_hz/PWM_FREQ_HZ) )) >>1;
    PAC55XX_TIMERA->CCTR4.CTR = val;
}
PAC5XXX_RAMFUNC static inline void m1_v_set_duty(float duty)
{
    uint16_t val = ((uint16_t)(duty * (timer_freq_hz/PWM_FREQ_HZ) )) >>1;
    PAC55XX_TIMERA->CCTR5.CTR = val;
}
PAC5XXX_RAMFUNC static inline void m1_w_set_duty(float duty)
{
    uint16_t val = ((uint16_t)(duty * (timer_freq_hz/PWM_FREQ_HZ) )) >>1;
    PAC55XX_TIMERA->CCTR6.CTR = val;
}

#endif /* GATEDRIVER_GATEDRIVER_H_ */
