
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

#include <src/motor/motor.hpp>
#include <src/observer/observer.hpp>

PAC5XXX_RAMFUNC uint8_t Motor::FindPolePairs(uint16_t cpr, float start_cpr, float end_cpr, float e_angle)
{
    const float pos = fabsf(end_cpr - start_cpr);
    float p_angle = twopi * pos/cpr;
    const float pairs_f = e_angle / p_angle;
    const uint8_t pairs_i = (uint8_t)floorf(pairs_f + 0.5f);
    const float residual = fabsf(pairs_f - (float)pairs_i);

    bool found = false;
    if (residual <= 0.30f)
    {
        found = true;
        Motor::SetPolePairs(pairs_i);
    }
    return found;
}

PAC5XXX_RAMFUNC uint8_t Motor::GetPolePairs(void)
{
    return config.pole_pairs;
}

PAC5XXX_RAMFUNC void Motor::SetPolePairs(uint8_t pairs)
{
    if (pairs > 1u)
    {
        config.pole_pairs = pairs;
        config.poles_calibrated = true;
    }
    // TODO: else error
}

PAC5XXX_RAMFUNC float Motor::GetPhaseResistance(void)
{
    return config.phase_resistance;
}
PAC5XXX_RAMFUNC void Motor::SetPhaseResistance(float R)
{
    if ((R > MIN_PHASE_RESISTANCE) && (R < MAX_PHASE_RESISTANCE))
    {
        config.phase_resistance = R;
        config.resistance_calibrated = true;
    }
    // TODO: else error
}

PAC5XXX_RAMFUNC float Motor::GetPhaseInductance(void)
{
    return config.phase_inductance;
}
PAC5XXX_RAMFUNC void Motor::SetPhaseInductance(float L)
{
    if ((L > MIN_PHASE_INDUCTANCE) && (L < MAX_PHASE_INDUCTANCE))
    {
        config.phase_inductance = L;
        config.inductance_calibrated = true;
    }
    // TODO: else error
}

PAC5XXX_RAMFUNC bool Motor::Calibrated(void)
{
    return config.resistance_calibrated && config.inductance_calibrated && config.poles_calibrated;
}
