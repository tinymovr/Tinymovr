
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

#include <src/adc/adc.hpp>
#include <src/controller/controller.hpp>
#include <src/encoder/encoder.hpp>
#include <src/nvm/nvm.hpp>
#include <src/observer/observer.hpp>
#include <src/system.hpp>
#include <src/watchdog/watchdog.hpp>
#include <string>

#define EP_LIST_SIZE 64
#define EP_MAP_SIZE 256

CANEP_Callback EPList[EP_LIST_SIZE];
uint8_t currentIndex = 0;
uint8_t EPMap[EP_MAP_SIZE] = {0};

void CAN::InitEndpointMap(void)
{
//    AddEndpoint(&CAN_EStop, 0x002);
//    AddEndpoint(&CAN_GetState, 0x003);
//    // 0x004 AVAIL
//    AddEndpoint(&CAN_GetCANConfig, 0x005);
//    AddEndpoint(&CAN_SetCANConfig, 0x006);
//    AddEndpoint(&CAN_SetState, 0x007);
//    // 0x008 AVAIL
//    AddEndpoint(&CAN_GetEncoderEstimates, 0x009);
//    AddEndpoint(&CAN_GetSetpoints, 0x00A);
//    // 0x00B Reserved: Move To Pos
//    AddEndpoint(&CAN_SetPosSetpoint, 0x00C);
//    AddEndpoint(&CAN_SetVelSetpoint, 0x00D);
//    AddEndpoint(&CAN_SetIqSetpoint, 0x00E);
//    AddEndpoint(&CAN_SetLimits, 0x00F);
//    AddEndpoint(&CAN_GetPhaseCurrents, 0x010);
//    // 0x011 Reserved: Move To Pos
//    // 0x012 Reserved: Move To Pos
//    // 0x013 AVAIL
//    AddEndpoint(&CAN_GetIq, 0x014);
//    AddEndpoint(&CAN_GetLimits, 0x015);
//    AddEndpoint(&CAN_Reset, 0x016);
//    AddEndpoint(&CAN_GetVBus, 0x017);
//    AddEndpoint(&CAN_GetGains, 0x018);
//    AddEndpoint(&CAN_SetGains, 0x019);
//    AddEndpoint(&CAN_DeviceInfo, 0x01A);
//    AddEndpoint(&CAN_Timings, 0x01B);
//    AddEndpoint(&CAN_SaveConfig, 0x01C);
//    AddEndpoint(&CAN_EraseConfig, 0x01D);
//    AddEndpoint(&CAN_GetMotorInfo, 0x01E);
//    // 0x01F AVAIL
}

void CAN::AddEndpoint(CANEP_Callback callback, uint8_t id)
{
	if (EPMap[id] == 0u)
	{
		currentIndex += 1u;
		EPMap[id] = currentIndex;
		EPList[currentIndex] = callback;
	}
}
CANEP_Callback CAN::GetEndpoint(uint8_t id)
{

}

//void CAN::AddEndpoint(Func<uint8_t> callback, uint8_t id)
//{
//    if (EPMap[id] == 0u)
//    {
//        currentIndex += 1u;
//        EPMap[id] = currentIndex;
//        EPList[currentIndex] = callback;
//    }
//}
//
//Func<uint8_t> CAN::GetEndpoint(uint8_t id)
//{
//	Func<uint8_t> cb = NULL;
//    if (EPMap[id] != 0u)
//    {
//        cb = EPList[EPMap[id]];
//    }
//    return cb;
//}
//
//// Endpoint handlers
//
//uint8_t CAN::CAN_EStop<uint8_t>(uint8_t buffer[])
//{
//	System::getInstance().controller.SetState(STATE_IDLE);
//    return CANRP_Write;
//}
//
//uint8_t CAN::CAN_GetState(uint8_t buffer[])
//{
//    uint8_t error = System::getInstance().controller.GetError();
//    uint8_t state = System::getInstance().controller.GetState();
//    uint8_t mode = System::getInstance().controller.GetMode();
//    memcpy(&buffer[0], &error, sizeof(uint8_t));
//    memcpy(&buffer[1], &state, sizeof(uint8_t));
//    memcpy(&buffer[2], &mode, sizeof(uint8_t));
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_GetCANConfig(uint8_t buffer[])
//{
//    uint8_t id = GetID();
//    uint16_t baudrate = GetkBaudRate();
//    memcpy(&buffer[0], &id, sizeof(uint8_t));
//    memcpy(&buffer[1], &baudrate, sizeof(uint16_t));
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_SetCANConfig(uint8_t buffer[])
//{
//    uint8_t id;
//    uint16_t baudrate;
//    memcpy(&id, &buffer[0], sizeof(uint8_t));
//    memcpy(&baudrate, &buffer[1], sizeof(uint16_t));
//    CAN_ResponseType response = CANRP_NoAction;
//    if (id >= 1u)
//    {
//        SetID(id);
//        response = CANRP_Write;
//    }
//    if ((baudrate >= 50u) && (baudrate <= 1000u))
//    {
//        SetkBaudRate(baudrate);
//        response = CANRP_Write;
//    }
//    return response;
//}
//
//uint8_t CAN::CAN_SetState(uint8_t buffer[])
//{
//	ControlState requested_state;
//	ControlMode requested_mode;
//    memcpy(&requested_state, &buffer[0], sizeof(uint8_t));
//    memcpy(&requested_mode, &buffer[1], sizeof(uint8_t));
//    CAN_ResponseType response = CANRP_NoAction;
//    if (requested_state <= STATE_CLOSED_LOOP_CONTROL)
//    {
//    	System::getInstance().controller.SetState(requested_state);
//        response = CANRP_Write;
//    }
//    if (requested_mode <= CTRL_POSITION)
//    {
//    	System::getInstance().controller.SetMode(requested_mode);
//        response = CANRP_Write;
//    }
//    return response;
//}
//
//uint8_t CAN::CAN_GetEncoderEstimates(uint8_t buffer[])
//{
//    float pos = System::getInstance().observer.GetPosEstimate();
//    float vel = System::getInstance().observer.GetVelEstimate();
//    memcpy(&buffer[0], &pos, sizeof(float));
//    memcpy(&buffer[4], &vel, sizeof(float));
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_GetSetpoints(uint8_t buffer[])
//{
//    float pos = System::getInstance().controller.GetPosSetpoint();
//    float vel = System::getInstance().controller.GetVelSetpoint();
//    memcpy(&buffer[0], &pos, sizeof(float));
//    memcpy(&buffer[4], &vel, sizeof(float));
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_SetPosSetpoint(uint8_t buffer[])
//{
//    float pos;
//    int16_t vel_ff;
//    int16_t Iq_ff;
//    memcpy(&pos, &buffer[0], sizeof(float));
//    memcpy(&vel_ff, &buffer[4], sizeof(int16_t));
//    memcpy(&Iq_ff, &buffer[6], sizeof(int16_t));
//    float velFF_float = vel_ff * 10.0f;
//    float iqFF_float = Iq_ff * 0.01f;
//    System::getInstance().controller.SetPosSetpoint(pos);
//    System::getInstance().controller.SetVelSetpoint(velFF_float);
//    System::getInstance().controller.SetIqSetpoint(iqFF_float);
//    return CANRP_Write;
//}
//
//uint8_t CAN::CAN_SetVelSetpoint(uint8_t buffer[])
//{
//    float vel;
//    float Iq_ff;
//    memcpy(&vel, &buffer[0], sizeof(float));
//    memcpy(&Iq_ff, &buffer[4], sizeof(float));
//    System::getInstance().controller.SetVelSetpoint(vel);
//    System::getInstance().controller.SetIqSetpoint(Iq_ff);
//    return CANRP_Write;
//}
//
//uint8_t CAN::CAN_SetIqSetpoint(uint8_t buffer[])
//{
//    float Iq;
//    memcpy(&Iq, &buffer[0], sizeof(float));
//    System::getInstance().controller.SetIqSetpoint(Iq);
//    return CANRP_Write;
//}
//
//uint8_t CAN::CAN_GetLimits(uint8_t buffer[])
//{
//    float vel_limit = System::getInstance().controller.GetVelLimit();
//    float iq_limit = System::getInstance().controller.GetIqLimit();
//    memcpy(&buffer[0], &vel_limit, sizeof(float));
//    memcpy(&buffer[4], &iq_limit, sizeof(float));
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_SetLimits(uint8_t buffer[])
//{
//    float vel_limit;
//    float iq_limit;
//    memcpy(&vel_limit, &buffer[0], sizeof(float));
//    memcpy(&iq_limit, &buffer[4], sizeof(float));
//    System::getInstance().controller.SetVelLimit(vel_limit);
//    System::getInstance().controller.SetIqLimit(iq_limit);
//    return CANRP_Write;
//}
//
//uint8_t CAN::CAN_GetPhaseCurrents(uint8_t buffer[])
//{
//    struct FloatTriplet I_phase;
//    System::getInstance().adc.GetPhaseCurrents(&I_phase);
//    int16_t IA = (int16_t)(I_phase.A * 1000.0f);
//    int16_t IB = (int16_t)(I_phase.B * 1000.0f);
//    int16_t IC = (int16_t)(I_phase.C * 1000.0f);
//    memcpy(&buffer[0], &IA, sizeof(int16_t));
//    memcpy(&buffer[2], &IB, sizeof(int16_t));
//    memcpy(&buffer[4], &IC, sizeof(int16_t));
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_GetIq(uint8_t buffer[])
//{
//    float Iq_set = System::getInstance().controller.GetIqSetpoint();
//    float Iq_est = System::getInstance().controller.GetIqEstimate();
//    memcpy(&buffer[0], &Iq_set, sizeof(float));
//    memcpy(&buffer[4], &Iq_est, sizeof(float));
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_Reset(uint8_t buffer[])
//{
//	System::getInstance().Reset();
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_GetVBus(uint8_t buffer[])
//{
//    const float VBus = System::getInstance().adc.GetVBus();
//    memcpy(&buffer[0], &VBus, sizeof(float));
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_GetGains(uint8_t buffer[])
//{
//    const float pos_gain = System::getInstance().controller.GetPosGain();
//    const float vel_P_gain = System::getInstance().controller.GetVelGain();
//    memcpy(&buffer[0], &pos_gain, sizeof(float));
//    memcpy(&buffer[4], &vel_P_gain, sizeof(float));
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_SetGains(uint8_t buffer[])
//{
//    float pos_gain;
//    float vel_P_gain;
//    memcpy(&pos_gain, &buffer[0], sizeof(float));
//    memcpy(&vel_P_gain, &buffer[4], sizeof(float));
//    CAN_ResponseType response = CANRP_NoAction;
//    if (pos_gain > 0.0f)
//    {
//    	System::getInstance().controller.SetPosGain(pos_gain);
//        response = CANRP_Write;
//    }
//    if (vel_P_gain > 0.0f)
//    {
//    	System::getInstance().controller.SetVelGain(vel_P_gain);
//        response = CANRP_Write;
//    }
//    return response;
//}
//
//uint8_t CAN::CAN_DeviceInfo(uint8_t buffer[])
//{
//    const uint32_t idr = PAC55XX_INFO2->PACIDR;
//    static const uint8_t v_major = VERSION_MAJOR;
//    static const uint8_t v_minor = VERSION_MINOR;
//    static const uint8_t v_patch = VERSION_PATCH;
//    const int8_t temp = (int8_t)System::getInstance().adc.GetMCUTemp();
//    memcpy(&buffer[0], &idr, sizeof(uint32_t));
//    memcpy(&buffer[4], &v_major, sizeof(uint8_t));
//    memcpy(&buffer[5], &v_minor, sizeof(uint8_t));
//    memcpy(&buffer[6], &v_patch, sizeof(uint8_t));
//    memcpy(&buffer[7], &temp, sizeof(int8_t));
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_Timings(uint8_t buffer[])
//{
//    const uint32_t delta = System::getInstance().controller.GetTotalCycles();
//    const uint32_t tpc = System::getInstance().controller.GetBusyCycles();
//    memcpy(&buffer[0], &delta, sizeof(uint32_t));
//    memcpy(&buffer[4], &tpc, sizeof(uint32_t));
//    return CANRP_Read;
//}
//
//uint8_t CAN::CAN_SaveConfig(uint8_t buffer[])
//{
//    NVM_SaveConfig();
//    return CANRP_Write;
//}
//
//uint8_t CAN::CAN_EraseConfig(uint8_t buffer[])
//{
//    NVM_Erase();
//    return CANRP_Write;
//}
//
//uint8_t CAN::CAN_GetMotorInfo(uint8_t buffer[])
//{
//    bool calibrated = System::getInstance().motor.Calibrated();
//    uint16_t R = (uint16_t)(System::getInstance().motor.GetPhaseResistance() * 1000);
//    uint8_t pole_pairs = System::getInstance().motor.GetPolePairs();
//    uint16_t L = (uint16_t)(System::getInstance().motor.GetPhaseInductance() * 1000000);
//    uint16_t cpr = (uint16_t)ENCODER_CPR;
//    memcpy(&buffer[0], &calibrated, sizeof(bool));
//    memcpy(&buffer[1], &R, sizeof(uint16_t));
//    memcpy(&buffer[3], &pole_pairs, sizeof(uint8_t));
//    memcpy(&buffer[4], &L, sizeof(uint16_t));
//    memcpy(&buffer[6], &cpr, sizeof(uint16_t));
//    return CANRP_Read;
//}
