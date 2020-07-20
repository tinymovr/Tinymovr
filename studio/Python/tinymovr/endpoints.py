
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

from tinymovr.iface import DataType

Endpoints = {
    "nmt":
    {
        "description": "CANOpen NMT Message",
        "type": "w",
        "ep_id": 0x000
    },
    "heartbeat":
    {
        "description": "CANOpen Heartbeat Message",
        "type": "w",
        "ep_id": 0x700
    },
    "estop":
    {
        "description": "Tinymovr Estop Message",
        "type": "w",
        "ep_id": 0x002
    },
    "state":
    {
        "description": "Get Tinymovr State, Control Mode and Error Code",
        "type": "r",
        "ep_id": 0x003,
        "types": (DataType.UINT8, DataType.UINT8, DataType.UINT8)
    },
    "can_config":
    {
        "description": "Get CAN Config",
        "type": "r",
        "ep_id": 0x005,
        "types": (DataType.UINT8, DataType.UINT16)
    },
    "set_can_config":
    {
        "description": "Set CAN Config",
        "type": "w",
        "ep_id": 0x006,
        "types": (DataType.UINT8, DataType.UINT16),
        "defaults": (0,)
    },
    "set_state":
    {
        "description": "Set Tinymovr State and optionally Control Mode",
        "type": "w",
        "ep_id": 0x007,
        "types": (DataType.UINT8, DataType.UINT8),
        "defaults": (0,)
    },
    "encoder_estimates":
    {
        "description": "Get Encoder Estimates (Position, Velocity)",
        "type": "r",
        "ep_id": 0x009,
        "types": (DataType.FLOAT, DataType.FLOAT)
    },
    "set_pos_setpoint":
    {
        "description": "Set Position Setpoint",
        "type": "w",
        "ep_id": 0x00C,
        "types": (DataType.FLOAT, DataType.INT16, DataType.INT16),
        "defaults": (0, 0)
    },
    "set_vel_setpoint":
    {
        "description": "Set Velocity Setpoint",
        "type": "w",
        "ep_id": 0x00D,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "defaults": (0,)
    },
    "set_cur_setpoint":
    {
        "description": "Set Current Setpoint",
        "type": "w",
        "ep_id": 0x00E,
        "types": (DataType.FLOAT,)
    },
    "set_limits":
    {
        "description": "Set Limits (Velocity, Current)",
        "type": "w",
        "ep_id": 0x00F,
        "types": (DataType.FLOAT, DataType.FLOAT)
    },
    "Iq":
    {
        "description": "Get Iq Estimate and Setpoint",
        "type": "r",
        "ep_id": 0x014,
        "types": (DataType.FLOAT, DataType.FLOAT)
    },
    "limits":
    {
        "description": "Get velocity and Iq limits",
        "type": "r",
        "ep_id": 0x015,
        "types": (DataType.FLOAT, DataType.FLOAT)
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
        "types": (DataType.FLOAT,)
    },
    "gains":
    {
        "description": "Get Gains (Position, Velocity)",
        "type": "r",
        "ep_id": 0x018,
        "types": (DataType.FLOAT, DataType.FLOAT)
    },
    "set_gains":
    {
        "description": "Set Gains (Velocity, Current)",
        "type": "w",
        "ep_id": 0x019,
        "types": (DataType.FLOAT, DataType.FLOAT)
    },
    "device_info":
    {
        "description": "Get Device Info",
        "type": "r",
        "ep_id": 0x01A,
        "types": (DataType.UINT8, DataType.UINT8, DataType.UINT8, DataType.UINT8, DataType.UINT16)
    },
    "timings":
    {
        "description": "Get Processor Timings (Busy Cycles/PWM, Cycles/PWM)",
        "type": "r",
        "ep_id": 0x01B,
        "types": (DataType.UINT32, DataType.UINT32)
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
    "motor_info":
    {
        "description": "Get Attached Motor Info (Calibrated, Resistance, Pole Pairs, Inductance)",
        "type": "r",
        "ep_id": 0x01E,
        "types": (DataType.UINT8, DataType.UINT16, DataType.UINT8, DataType.FLOAT)
    },
}