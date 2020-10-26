
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

#include <src/component.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include <src/adc/adc_consts.h>

#ifdef __cplusplus
}
#endif


class ADC : public Component
{
public:
    ADC(System sys_);
    PAC5XXX_RAMFUNC float GetVBus(void);
    PAC5XXX_RAMFUNC int16_t GetMCUTemp(void);
    PAC5XXX_RAMFUNC void GetPhaseCurrents(struct FloatTriplet *phc);
    PAC5XXX_RAMFUNC void UpdateCurrentMeas(void);
private:
    struct ADCConfig
    {
        float Iphase_limit = 40.0f;
        float I_filter_k = 0.6f;
        float I_phase_offset_tau = 0.2f;
    };
    ADCConfig config;
    float vbus;
    int16_t temp;
    struct FloatTriplet I_phase_meas;
    struct FloatTriplet I_phase_offset;

    void AIO_Init(void);
    void DTSE_Init(void);
};

#endif /* ADC_ADC_H_ */
