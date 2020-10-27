
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

#ifndef CONTROLLER_CONTROLLER_H_
#define CONTROLLER_CONTROLLER_H_

#include "src/component.hpp"

typedef enum {
	STATE_IDLE = 0,
	STATE_CALIBRATE = 1,
    STATE_CLOSED_LOOP_CONTROL = 2
} ControlState;

typedef enum {
	CTRL_CURRENT = 0,
	CTRL_VELOCITY = 1,
    CTRL_POSITION = 2
} ControlMode;

typedef enum {
	ERROR_NO_ERROR = 0,
	ERROR_INVALID_STATE = 1,
	ERROR_ILLEGAL_VALUE = 2,
	ERROR_VBUS_UNDERVOLTAGE = 3,
	ERROR_OVERCURRENT = 4,
    ERROR_PWM_LIMIT_EXCEEDED = 5,
	// Keep below last to make comparisons for state change
	ERROR_PHASE_RESISTANCE_OUT_OF_RANGE = 6,
    ERROR_PHASE_INDUCTANCE_OUT_OF_RANGE = 7,
    ERROR_INVALID_POLE_PAIRS = 8
} ControlError;

class ControlAction
{
public:
	virtual void operator () () {};
	virtual ~ControlAction();
};

class Controller: public Component
{
public:
	struct ControllerConfig
	{
	    float vel_limit = 300000.0f;
	    float I_limit = 10.0f;

	    float pos_gain = 25.0f;
	    float vel_gain = 1.0e-4f;
	    float vel_integrator_gain = 0.0f;
	    float I_bw = 800.0;
	    float I_gain = 0.0f;
	    float Iq_integrator_gain = 0.0f;
	    float Id_integrator_gain = 0.0f;

	    float I_k = 0.3f;
	};

	ControllerConfig config;

	void HealthCheck(void);
	void ControlLoop(void);

	ControlState GetState(void);
	void SetState(ControlState new_state);

	ControlMode GetMode(void);
	void SetMode(ControlMode mode);

	float GetPosSetpoint(void);
	void SetPosSetpoint(float value);

	float GetVelSetpoint(void);
	void SetVelSetpoint(float value);

	float GetIqEstimate(void);

	float GetIqSetpoint(void);
	void SetIqSetpoint(float value);

	void GetModulationValues(struct FloatTriplet *dc);

	float GetPosGain(void);
	void SetPosGain(float gain);
	float GetVelGain(void);
	void SetVelGain(float gain);
	float GetVelIntegratorGain(void);
	void SetVelIntegratorGain(float gain);
	float GetIqGain(void);
	float GetIqBandwidth(void);
	void SetIqBandwidth(float bw);

	float GetVelLimit(void);
	void SetVelLimit(float limit);
	float GetIqLimit(void);
	void SetIqLimit(float limit);

	bool Calibrated(void);
	uint8_t GetError(void);

	uint32_t GetTotalCycles(void);
	uint32_t GetBusyCycles(void);

	void UpdateCurrentGains(void);
private:

	ControlState state = STATE_IDLE;
	ControlMode mode = CTRL_CURRENT;
	ControlError error = ERROR_NO_ERROR;

	struct FloatTriplet I_phase_meas = {0.0f, 0.0f, 0.0f};
	struct FloatTriplet modulation_values = {0.0f, 0.0f, 0.0f};

	float Iq_meas = 0.0f;
	float Id_meas = 0.0f;

	float pos_setpoint = 0.0f;
	float vel_setpoint = 0.0f;
	float Iq_setpoint = 0.0f;

	float vel_integrator_Iq = 0.0f;

	float Iq_integrator_Vq = 0.0f;
	float Id_integrator_Vd = 0.0f;

	uint32_t busy_cycles = 0;
	uint32_t total_cycles = 0;

	uint32_t last_timestamp = 0;

	void ClosedLoopControlStep();
	void IdleStep();

	bool LimitVelocity(float min_limit, float max_limit, float vel_estimate,
		    float vel_gain, float *I);
};

#endif /* CONTROLLER_CONTROLLER_H_ */
