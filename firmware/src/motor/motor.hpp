
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

#ifndef MOTOR_MOTOR_H_
#define MOTOR_MOTOR_H_

#include <src/common.hpp>

class Motor
{
public:
	PAC5XXX_RAMFUNC uint8_t GetPolePairs(void);
	PAC5XXX_RAMFUNC uint8_t FindPolePairs(uint16_t cpr, float start_cpr, float end_cpr, float e_angle);
	PAC5XXX_RAMFUNC void SetPolePairs(uint8_t pairs);

	PAC5XXX_RAMFUNC float GetPhaseResistance(void);
	PAC5XXX_RAMFUNC void SetPhaseResistance(float R);

	PAC5XXX_RAMFUNC float GetPhaseInductance(void);
	PAC5XXX_RAMFUNC void SetPhaseInductance(float L);

	PAC5XXX_RAMFUNC bool Calibrated(void);
private:
	struct MotorConfig
	{
		uint8_t pole_pairs = 7u;
		float phase_resistance = 0.1f;
		float phase_inductance = 1e-5f;

		bool resistance_calibrated = false;
		bool inductance_calibrated = false;
		bool poles_calibrated = false;
	};
	MotorConfig config;
}

#endif /* MOTOR_MOTOR_H_ */
