""" Tinymovr user wrapper module.

This module includes a friendly Tinymovr user wrapper class that makes it
easier for the user to issue commands to Tinymovr through the shell.

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

from tinymovr import Tinymovr
from tinymovr.constants import ControlStates, ControlModes

msg_calibrating = "Tinymovr is currently calibrating, please do not interrupt."


class UserWrapper:
    """
    This is a user-friendly wrapper to the Tinymover class, that
    allows more intuitive functions to be used instead of plain
    getters/setters.
    This class sacrifices Pythonic-ness (e.g. raising exceptions)
    for better user-friendliness (e.g. using simple print statements).
    There are also additional checks performed prior to changing
    states, with errors displayed if checks fail.
    """

    def __init__(self, *args, **kwargs):
        self.tinymovr = Tinymovr(*args, **kwargs)

    def __getattr__(self, attr):
        return getattr(self.tinymovr, attr)

    def calibrate(self):
        """
        Enter calibration state, perform motor and encoder calibration
        """
        state = self.tinymovr.state
        if state.errors:
            print(
                "Error flag present, cannot continue with calibration. Please reset Tinymovr."
            )
        elif state.state != ControlStates.Idle:
            print("Tinymovr not in idle state, calibration cannot start.")
        else:
            input(
                "Ready to calibrate. Please remove any loads from the motor and hit Enter to continue."
            )
            self.tinymovr.calibrate()

    def idle(self):
        """
        Enter idle state
        """
        if self.tinymovr.state.state == ControlStates.Calibration:
            print(msg_calibrating)
        else:
            self.tinymovr.idle()

    def position_control(self):
        """
        Enter closed loop control state and position control mode
        """
        state = self.tinymovr.state
        if state.errors:
            print(
                "Error flag present, cannot enable position control. Please reset Tinymovr."
            )
        elif state.state == ControlStates.Calibration:
            print(msg_calibrating)
        else:
            self.tinymovr.position_control()

    def velocity_control(self):
        """
        Enter closed loop control state and velocity control mode
        """
        state = self.tinymovr.state
        if state.errors:
            print(
                "Error flag present, cannot enable velocity control. Please reset Tinymovr."
            )
        elif state.state == ControlStates.Calibration:
            print(msg_calibrating)
        else:
            self.tinymovr.velocity_control()

    def current_control(self):
        """
        Enter closed loop control state and current control mode
        """
        state = self.tinymovr.state
        if state.errors:
            print(
                "Error flag present, cannot enable current control. Please reset Tinymovr."
            )
        elif state.state == ControlStates.Calibration:
            print(msg_calibrating)
        else:
            self.tinymovr.current_control()

    def __dir__(self):
        tm_attrs = self.tinymovr.__dir__()
        self_attrs = [k for k in object.__dir__(self) if not k.startswith("_")]
        self_attrs.remove("tinymovr")
        return sorted(list(set(tm_attrs + self_attrs)))
