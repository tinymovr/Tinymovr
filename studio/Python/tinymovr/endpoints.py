
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
        "types": (DataType.UINT8, DataType.UINT8, DataType.UINT8),
        "labels": ("error", "state", "mode")
    },
    "can_config":
    {
        "description": "Get CAN Config",
        "type": "r",
        "ep_id": 0x005,
        "types": (DataType.UINT8, DataType.UINT16),
        "labels": ("id", "baud_rate")
    },
    "set_can_config":
    {
        "description": "Set CAN Config",
        "type": "w",
        "ep_id": 0x006,
        "types": (DataType.UINT8, DataType.UINT16),
        "defaults": (0,),
        "labels": ("id", "baud_rate")
    },
    "set_state":
    {
        "description": "Set Tinymovr State and optionally Control Mode",
        "type": "w",
        "ep_id": 0x007,
        "types": (DataType.UINT8, DataType.UINT8),
        "defaults": (0,),
        "labels": ("state", "mode")
    },
    "encoder_estimates":
    {
        "description": "Get Encoder Estimates (Position, Velocity)",
        "type": "r",
        "ep_id": 0x009,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "labels": ("position", "velocity")
    },
    "setpoints":
    {
        "description": "Get Setpoints (Position, Velocity)",
        "type": "r",
        "ep_id": 0x009,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "labels": ("position", "velocity")
    },
    "set_pos_setpoint":
    {
        "description": "Set Position Setpoint",
        "type": "w",
        "ep_id": 0x00C,
        "types": (DataType.FLOAT, DataType.INT16, DataType.INT16),
        "defaults": (0, 0),
        "labels": ("position", "velocity_ff", "current_ff")
    },
    "set_vel_setpoint":
    {
        "description": "Set Velocity Setpoint",
        "type": "w",
        "ep_id": 0x00D,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "defaults": (0,),
        "labels": ("velocity", "current_ff")
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
        "types": (DataType.FLOAT, DataType.FLOAT),
        "labels": ("velocity", "current")
    },
    "Iq":
    {
        "description": "Get current setpoint and estimate",
        "type": "r",
        "ep_id": 0x014,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "labels": ("setpoint", "estimate")
    },
    "limits":
    {
        "description": "Get velocity and current limits",
        "type": "r",
        "ep_id": 0x015,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "labels": ("velocity", "current")
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
        "types": (DataType.FLOAT, DataType.FLOAT),
        "labels": ("position", "velocity")
    },
    "set_gains":
    {
        "description": "Set Gains (Position, Velocity)",
        "type": "w",
        "ep_id": 0x019,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "labels": ("position", "velocity")
    },
    "device_info":
    {
        "description": "Get Device Info",
        "type": "r",
        "ep_id": 0x01A,
        "types": ( DataType.UINT32, DataType.UINT8, DataType.UINT8, DataType.UINT8, DataType.UINT8,),
        "labels": ("device_id", "fw_major", "fw_minor", "fw_patch", "temp")
    },
    "timings":
    {
        "description": "Get Processor Timings (Busy Cycles/PWM, Total Cycles/PWM)",
        "type": "r",
        "ep_id": 0x01B,
        "types": (DataType.UINT32, DataType.UINT32),
        "labels": ("busy", "total")
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
        "description": "Get Attached Motor Info (Calibrated, Resistance, Pole Pairs, Inductance, enc CPR)",
        "type": "r",
        "ep_id": 0x01E,
        "types": (DataType.UINT8, DataType.UINT16, DataType.UINT8, DataType.UINT16, DataType.UINT16),
        "labels": ("calibrated", "R", "pole_pairs", "L", "encoder_cpr")
    },
}