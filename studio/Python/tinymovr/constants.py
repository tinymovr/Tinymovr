"""
Copyright 2022 Ioannis Chatzikonstantinou (Tinymovr)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
