
from enum import IntEnum

class ControlStates(IntEnum):
    Idle = 0
    Calibration = 1
    ClosedLoopControl = 2

class ControlModes(IntEnum):
    CurrentControl = 0
    VelocityControl = 1
    PositionControl = 2

class ErrorIDs(IntEnum):
    InvalidState = 1
    IllegalValue = 2
    VBusUndervoltage = 3
    OverCurrent = 4
    PWMLimitExceeded = 5
    PhaseResistanceOutOfRange = 6
    PhaseInductanceOutOfRange = 7
    InvalidPolePairs = 8
    EncoderReadingOutOfRange = 9


error_descriptions = {
    ErrorIDs.InvalidState: "Attempt to transition to invalid state",
    ErrorIDs.IllegalValue: "Illegal value passed to endpoint",
    ErrorIDs.VBusUndervoltage: "Power supply bus under low voltage threshold",
    ErrorIDs.OverCurrent: "Phase current limit has been exceeded",
    ErrorIDs.PWMLimitExceeded: "Phase voltage modulation limit has been exceeded",
    ErrorIDs.PhaseResistanceOutOfRange: "Measured phase resistance is out of range",
    ErrorIDs.PhaseInductanceOutOfRange: "Measured phase inductance is out of range",
    ErrorIDs.InvalidPolePairs: "Pole pairs calculation did not converge to a valid value",
    ErrorIDs.EncoderReadingOutOfRange: "The encoder angle reading is out of range"
}    