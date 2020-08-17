
from enum import Enum

class ErrorIDs(Enum):
    NoError = 0
    InvalidState = 1
    IllegalValue = 2
    VBusUndervoltage = 3
    OverCurrent = 4
    PWMLimitExceeded = 5
    PhaseResistanceOutOfRange = 6
    PhaseInductanceOutOfRange = 7
    InvalidPolePairs = 8
    MagneticFieldOutOfRange = 9

error_descriptions = {
    ErrorIDs.NoError: "No error",
    ErrorIDs.InvalidState: "Attempt to transition to invalid state",
    ErrorIDs.IllegalValue: "Illegal value passed to endpoint",
    ErrorIDs.VBusUndervoltage: "Power supply bus under low voltage threshold",
    ErrorIDs.OverCurrent: "Phase current limit has been exceeded",
    ErrorIDs.PWMLimitExceeded: "Phase voltage modulation limit has been exceeded",
    ErrorIDs.PhaseResistanceOutOfRange: "Measured phase resistance is out of range",
    ErrorIDs.PhaseInductanceOutOfRange: "Measured phase inductance is out of range",
    ErrorIDs.InvalidPolePairs: "Pole pairs calculation did not converge to a valid value",
    ErrorIDs.MagneticFieldOutOfRange: "The magnetic field sensed by the encoder is out of range"
}    