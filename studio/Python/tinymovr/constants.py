from enum import IntEnum


app_name = "Tinymovr Studio"
base_node_name = "tm"


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
