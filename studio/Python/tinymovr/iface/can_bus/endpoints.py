
''' Tinymovr endpoints definitions module.

This module includes definitions for all Tinymovr endpoints .

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
'''
from typing import Dict
from tinymovr.codec import DataType
from tinymovr.units import get_registry

can_endpoints: Dict[str, Dict] = {
    "offset_dir":
    {
        "description": "Get User-defined Offset and Direction",
        "type": "r",
        "ep_id": 0x002,
        "types": (DataType.FLOAT, DataType.INT8),
        "labels": ("offset", "direction")
    },
    "state":
    {
        "description": "Get Tinymovr State, Control Mode and Error Code",
        "type": "r",
        "ep_id": 0x003,
        "types": [DataType.UINT8] * 8,
        "labels": ("error", "state", "mode", "error0", "error1", "error2", "error3", "error4")
    },
    "min_studio_version":
    {
        "description": "Get the minimum Studio Version required by the Firmware",
        "type": "r",
        "ep_id": 0x004,
        "types": (DataType.UINT8, DataType.UINT8, DataType.UINT8),
        "labels": ("fw_major", "fw_minor", "fw_patch")
    },
    "can_config":
    {
        "description": "Get CAN Config",
        "type": "r",
        "ep_id": 0x005,
        "types": (DataType.UINT8, DataType.UINT16),
        "labels": ("id", "baud_rate"),
        "ser_map": {"can": ("id", "baud_rate")}
    },
    "set_can_config":
    {
        "description": "Set CAN Config",
        "type": "w",
        "ep_id": 0x006,
        "types": (DataType.UINT8, DataType.UINT16),
        "defaults": {"baud_rate": 0},
        "labels": ("id", "baud_rate"),
        "ser_map": {"can": ("id", "baud_rate")}
    },
    "set_state":
    {
        "description": "Set Tinymovr State and optionally Control Mode",
        "type": "w",
        "ep_id": 0x007,
        "types": (DataType.UINT8, DataType.UINT8),
        "defaults": {"mode": 0},
        "labels": ("state", "mode")
    },
    "set_offset_dir":
    {
        "description": "Set User-defined Offset and Direction",
        "type": "w",
        "ep_id": 0x008,
        "types": (DataType.FLOAT, DataType.INT8),
        "labels": ("offset", "direction")
    },
    "encoder_estimates":
    {
        "description": "Get Encoder Estimates (Position, Velocity)",
        "type": "r",
        "ep_id": 0x009,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("tick", "tick/second"),
        "labels": ("position", "velocity")
    },
    "setpoints":
    {
        "description": "Get Setpoints (Position, Velocity)",
        "type": "r",
        "ep_id": 0x00A,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("tick", "tick/second"),
        "labels": ("position", "velocity")
    },
    "encoder_config":
    {
        "description": "Get encoder config",
        "type": "r",
        "ep_id": 0x00B,
        "types": (DataType.UINT8, DataType.FLOAT),
        "units": (None, "radians/second"),
        "labels": ("type", "bandwidth")
    },
    "set_pos_setpoint":
    {
        "description": "Set Position Setpoint",
        "type": "w",
        "ep_id": 0x00C,
        "types": (DataType.FLOAT, DataType.INT16, DataType.INT16),
        "units": ("tick", "decatick/second", "centiampere"),
        "defaults": {"velocity_ff": 0, "current_ff": 0},
        "labels": ("position", "velocity_ff", "current_ff")
    },
    "set_vel_setpoint":
    {
        "description": "Set Velocity Setpoint",
        "type": "w",
        "ep_id": 0x00D,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("tick/second", "ampere"),
        "defaults": {"current_ff": 0},
        "labels": ("velocity", "current_ff")
    },
    "set_cur_setpoint":
    {
        "description": "Set Current (Iq) Setpoint",
        "type": "w",
        "ep_id": 0x00E,
        "types": (DataType.FLOAT,),
        "units": ("ampere",),
        "labels": ("current",)
    },
    "set_limits":
    {
        "description": "Set Limits (Velocity, Current)",
        "type": "w",
        "ep_id": 0x00F,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("tick/second", "ampere"),
        "labels": ("velocity", "current"),
        "ser_map": {"limits": ("velocity", "current")}
    },
    "Iphase":
    {
        "description": "Get measured phase currents",
        "type": "r",
        "ep_id": 0x010,
        "types": (DataType.INT16, DataType.INT16, DataType.INT16),
        "units": ("milliampere", "milliampere", "milliampere"),
        "labels": ("I_A", "I_B", "I_C"),
        "from_version": "0.7.1"
    },
    "set_encoder_config":
    {
        "description": "Set encoder configuration",
        "type": "w",
        "ep_id": 0x011,
        "types": (DataType.UINT8, DataType.FLOAT),
        "labels": ("type", "bandwidth")
    },
    "vel_integrator_params":
    {
        "description": "Get velocity integrator gain and deadband parameters",
        "type": "r",
        "ep_id": 0x012,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("(A*second)/tick", "tick"),
        "labels": ("gain", "deadband"),
        "ser_map": {"vel_integrator_params": ("gain", "deadband")}
    },
    "set_vel_integrator_params":
    {
        "description": "Set velocity integrator gain and deadband parameters",
        "type": "w",
        "ep_id": 0x013,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("(A*second)/tick", "tick"),
        "labels": ("gain", "deadband"),
        "ser_map": {"vel_integrator_params": ("gain", "deadband")}
    },
    "Iq":
    {
        "description": "Get quadrature current setpoint and estimate",
        "type": "r",
        "ep_id": 0x014,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("ampere", "ampere"),
        "labels": ("setpoint", "estimate")
    },
    "limits":
    {
        "description": "Get velocity and current limits",
        "type": "r",
        "ep_id": 0x015,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("tick/second", "ampere"),
        "labels": ("velocity", "current"),
        "ser_map": {"limits": ("velocity", "current")}
    },
    "reset":
    {
        "description": "Tinymovr Reset Message",
        "type": "w",
        "ep_id": 0x016
    },
    "Vbus":
    {
        "description": "Get Bus Voltage",
        "type": "r",
        "ep_id": 0x017,
        "units": ("volt",),
        "types": (DataType.FLOAT,)
    },
    "gains":
    {
        "description": "Get Gains (Position, Velocity)",
        "type": "r",
        "ep_id": 0x018,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("1/second", "(A*second)/tick"),
        "labels": ("position", "velocity"),
        "ser_map": {"gains": ("position", "velocity")}
    },
    "set_gains":
    {
        "description": "Set Gains (Position, Velocity)",
        "type": "w",
        "ep_id": 0x019,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("1/second", "(A*second)/tick"),
        "labels": ("position", "velocity"),
        "ser_map": {"gains": ("position", "velocity")}
    },
    "device_info":
    {
        "description": "Get Device Info",
        "type": "r",
        "ep_id": 0x01A,
        "types": (DataType.UINT32, DataType.UINT8,
                  DataType.UINT8, DataType.UINT8, DataType.UINT8),
        "labels": ("device_id", "fw_major", "fw_minor", "fw_patch", "temp")
    },
    "timings":
    {
        "description": "Get Processor Timings \
(Total Cycles/PWM, Busy Cycles/PWM)",
        "type": "r",
        "ep_id": 0x01B,
        "types": (DataType.UINT32, DataType.UINT32),
        "labels": ("total", "busy")
    },
    "save_config":
    {
        "description": "Save Configuration",
        "type": "w",
        "ep_id": 0x01C
    },
    "erase_config":
    {
        "description": "Erase Configuration",
        "type": "w",
        "ep_id": 0x01D
    },
    "motor_config":
    {
        "description": "Get Motor Configuration (Flags (calibrated, is_gimbal), \
 Pole Pairs, Calibration Current)",
        "type": "r",
        "ep_id": 0x01E,
        "types": (DataType.UINT8, DataType.UINT8, DataType.FLOAT),
        "labels": ("flags", "pole_pairs", "I_cal"),
        "units": (None, None, "ampere"),
        "ser_map": {"motor": ("pole_pairs", "I_cal")}
    },
    "set_motor_config":
    {
        "description": "Set Motor Configuration \
                        (Flags (is_gimbal), Pole Pairs, Calibration Current)",
        "type": "w",
        "ep_id": 0x01F,
        "types": (DataType.UINT8, DataType.UINT8, DataType.FLOAT),
        "labels": ("flags", "pole_pairs", "I_cal"),
        "units": (None, None, "ampere"),
        "ser_map": {"motor": ("pole_pairs", "I_cal")}
    },
    "plan_t_limit":
    {
        "description": "Execute a planned move to pos with time limit",
        "type": "w",
        "ep_id": 0x020,
        "types": (DataType.FLOAT, DataType.UINT16, DataType.UINT8, DataType.UINT8),
        "units": ("tick", "millisecond", None, None),
        "labels": ("target_position", "total_time", "acc_fraction", "dec_fraction")
    },
    "plan_v_limit":
    {
        "description": "Execute a planned move to pos with velocity limit",
        "type": "w",
        "ep_id": 0x021,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("tick", "tick/second"),
        "labels": ("target_position", "max_vel")
    },
    "set_max_plan_acc_dec":
    {
        "description": "Set maximum acceleration and deceleration \
for velocity-limited plan moves",
        "type": "w",
        "ep_id": 0x022,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("tick/second/second", "tick/second/second"),
        "labels": ("max_accel", "max_decel")
    },
    "get_max_plan_acc_dec":
    {
        "description": "Get maximum acceleration and deceleration \
for velocity-limited plan moves",
        "type": "r",
        "ep_id": 0x023,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("tick/second/second", "tick/second/second"),
        "labels": ("max_accel", "max_decel")
    },
    "get_set_pos_vel":
    {
        "description": "Get and set Position and Velocity feedforward in one go",
        "type": "rw",
        "ep_id": 0x025,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("tick", "tick/second"),
        "defaults": {"velocity_ff": 0},
        "labels": ("position", "velocity_ff")
    },
    "get_set_pos_vel_Iq":
    {
        "description": "Get and set Position, Velocity feedforward and Iq feedforward in one go",
        "type": "rw",
        "ep_id": 0x026,
        "types": (DataType.FLOAT, DataType.INT16, DataType.INT16),
        "units": ("tick", "decatick/second", "centiampere"),
        "defaults": {"velocity_ff": 0, "current_ff": 0},
        "labels": ("position", "velocity_ff", "current_ff")
    },
    "motor_RL":
    {
        "description": "Get Motor Resistance and Inductance values",
        "type": "r",
        "ep_id": 0x027,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "labels": ("R", "L"),
        "units": ("ohm", "henry"),
        "ser_map": {"motor": ("R", "L")}
    },
    "set_motor_RL":
    {
        "description": "Set Motor Resistance and Inductance",
        "type": "w",
        "ep_id": 0x028,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "labels": ("R", "L"),
        "units": ("ohm", "henry"),
        "ser_map": {"motor": ("R", "L")}
    },
    "hall_sector":
    {
        "description": "Get Hall effect sensor sector map and current sector",
        "type": "r",
        "ep_id": 0x029,
        "types": (DataType.UINT8, DataType.UINT8,
            DataType.UINT8, DataType.UINT8,
            DataType.UINT8, DataType.UINT8,
            DataType.UINT8, DataType.UINT8),
        "labels": ("s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "current")
    },
    "set_watchdog":
    {
        "description": "Enable/disable the CAN timeout watchdog and set its timeout in seconds. Maximum of 536s.",
        "type": "w",
        "ep_id": 0x02A,
        "types": (DataType.INT8, DataType.FLOAT),
        "units": ("state", "second"),
        "defaults": {"timeout": 3},
        "labels": ("enabled", "timeout")
    },
    "set_vel_inc":
    {
        "description": "Set velocity increment (ramp). Set to zero to disable.",
        "type": "w",
        "ep_id": 0x02B,
        "types": (DataType.FLOAT,),
        "labels": ("increment",),
        "defaults": {"increment": 100,}
    },
}
