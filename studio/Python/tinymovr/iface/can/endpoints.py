
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
        "units": ("tick", "tick/second"),
        "labels": ("position", "velocity"),
        "ser_map": {"gains": ("position", "velocity")}
    },
    "set_gains":
    {
        "description": "Set Gains (Position, Velocity)",
        "type": "w",
        "ep_id": 0x019,
        "types": (DataType.FLOAT, DataType.FLOAT),
        "units": ("tick", "tick/second"),
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
    "motor_info":
    {
        "description": "Get Attached Motor Info (Calibrated, \
                        Resistance, Pole Pairs, Inductance, Encoder Ticks)",
        "type": "r",
        "ep_id": 0x01E,
        "types": (DataType.UINT8, DataType.UINT16,
                  DataType.UINT8, DataType.UINT16, DataType.UINT16),
        "labels": ("calibrated", "R", "pole_pairs", "L", "encoder_ticks"),
        "units": (None, "milliohm", "microhenry", "ticks"),
        "ser_map": {"motor": ("R", "L", "pole_pairs")}
    },
}
