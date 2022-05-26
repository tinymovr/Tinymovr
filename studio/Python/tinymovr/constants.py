"""
This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
"""

import math
from enum import IntEnum


app_name = "Tinymovr Studio"
base_node_name = "tm"
HEARTBEAT_BASE = 0x700

CAN_EP_SIZE = 6
CAN_EP_MASK = 1 << CAN_EP_SIZE - 1
CAN_SEQ_SIZE = 6
CAN_SEQ_MASK = (1 << CAN_SEQ_SIZE - 1) << CAN_EP_SIZE
CAN_DEV_SIZE = 6
CAN_DEV_MASK = (1 << CAN_DEV_SIZE - 1) << (CAN_EP_SIZE + CAN_SEQ_SIZE)


class ControlStates(IntEnum):
    Idle = 0
    Calibration = 1
    ClosedLoopControl = 2


class ControlModes(IntEnum):
    CurrentControl = 0
    VelocityControl = 1
    PositionControl = 2
    TrajectoryControl = 3


class ErrorIDs(IntEnum):
    InvalidState = 1
    ControlBlockReentered = 2
    VBusUndervoltage = 3
    OverCurrent = 4
    PWMLimitExceeded = 5
    PhaseResistanceOutOfRange = 6
    PhaseInductanceOutOfRange = 7
    InvalidPolePairs = 8
    EncoderReadingOutOfRange = 9
    InvalidPlannerInput = 10
    PlannerVCruiseOverLimit = 11


error_descriptions = {
    ErrorIDs.InvalidState: "Attempt to transition to invalid state",
    ErrorIDs.ControlBlockReentered: "Control block re-entered before last instance completed",
    ErrorIDs.VBusUndervoltage: "Power supply bus under low voltage threshold",
    ErrorIDs.OverCurrent: "Phase current limit has been exceeded",
    ErrorIDs.PWMLimitExceeded: "Phase voltage modulation limit has been exceeded",
    ErrorIDs.PhaseResistanceOutOfRange: "Measured phase resistance is out of range",
    ErrorIDs.PhaseInductanceOutOfRange: "Measured phase inductance is out of range",
    ErrorIDs.InvalidPolePairs: "Pole pairs calculation did not converge to a valid value",
    ErrorIDs.EncoderReadingOutOfRange: "The encoder angle reading is out of range",
    ErrorIDs.InvalidPlannerInput: "The trajectory planner received invalid input",
    ErrorIDs.PlannerVCruiseOverLimit: "The computed trajectory planner cruise velocity is over the current velocity limit",
}
