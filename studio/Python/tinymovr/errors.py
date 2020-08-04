
from enum import Enum

class ErrorIDs(Enum):
    NoError = 0
    InvalidState = 1
    IllegalValue = 2
    VBusUndervoltage = 3
    OverCurrent = 4
    PhaseResistanceOutOfRange = 5
    PhaseInductanceOutOfRange = 6
    InvalidPolePairs = 7

error_descriptions = {
    ErrorIDs.NoError: "No error",
    ErrorIDs.InvalidState: "Attempt to transition to invalid state",
    ErrorIDs.IllegalValue: "Illegal value passed to endpoint",
    ErrorIDs.VBusUndervoltage: "Power supply bus under low voltage threshold",
    ErrorIDs.OverCurrent: "Phase current exceeded threshold",
    ErrorIDs.PhaseResistanceOutOfRange: "Measured phase resistance is out of range",
    ErrorIDs.PhaseInductanceOutOfRange: "Measured phase inductance is out of range",
    ErrorIDs.InvalidPolePairs: "Pole pairs calculation did not converge to a valid value"
}    