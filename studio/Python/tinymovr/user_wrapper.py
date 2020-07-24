
''' Tinymovr user wrapper module.

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
'''

from tinymovr import Tinymovr, ErrorIDs, error_descriptions

class UserWrapper:
    '''
    This is a user-friendly wrapper to the Tinymover class, that
    allows more intuitive functions to be used instead of plain
    getters/setters.
    This class sacrifices Pythonic-ness (e.g. raising exceptions)
    for better user-friendliness (e.g. using simple print statements).
    There are also additional checks performed prior to the 
    '''
    def __init__(self, **kwargs):
        self.tinymovr = Tinymovr(**kwargs)
        self._encoder_cpr = -1

    def __getattr__(self, attr):
        return getattr(self.tinymovr, attr)

    def calibrate(self):
        '''
        Enter calibration state, perform motor and encoder calibration
        '''
        state = self.tinymovr.state
        if state.error != 0:
            print("Error flag present, cannot continue with calibration. Please reset Tinymovr.")
        elif state.state != 0:
            print("Tinymovr state is not idle, calibration needs to be started from idle state.")
        else:
            input("Ready to calibrate. Please remove any loads from the motor and hit Enter to continue")
            self.tinymovr.calibrate()

    def idle(self):
        '''
        Enter idle state
        '''
        self.tinymovr.set_state(0)

    def position_control(self):
        '''
        Enter closed loop control state and position control mode
        '''
        state = self.tinymovr.state
        if state.error != 0:
            print("Error flag present, cannot enable position control. Please reset Tinymovr.")
        elif state.mode == 1:
            print("Tinymovr is currently calibrating, please do not interrupt.")
        else:
            self.tinymovr.position_control()

    def velocity_control(self):
        '''
        Enter closed loop control state and velocity control mode
        '''
        state = self.tinymovr.state
        if state.error != 0:
            print("Error flag present, cannot enable velocity control. Please reset Tinymovr.")
        elif state.mode == 1:
            print("Tinymovr is currently calibrating, please do not interrupt.")
        else:
            self.tinymovr.velocity_control()

    def current_control(self):
        '''
        Enter closed loop control state and current control mode
        '''
        state = self.tinymovr.state
        if state.error != 0:
            print("Error flag present, cannot enable current control. Please reset Tinymovr.")
        elif state.mode == 1:
            print("Tinymovr is currently calibrating, please do not interrupt.")
        else:
            self.tinymovr.current_control()

    def get_velocity_estimate_rpm(self, value):
        '''
        Get velocity estimate in RPM by automatically
        converting from encoder counts per second.

        NOTE: This convenience function exists only for
        velocity control mode and does not support
        current feed-forward value.
        '''
        return 60 * (self.tinymovr.encoder_estimates.velocity / self.encoder_cpr)

    def set_velocity_setpoint_rpm(self, value):
        '''
        Set velocity setpoint in RPM by automatically
        converting to encoder counts per second

        NOTE: This convenience function exists only for
        velocity control mode and does not support
        current feed-forward value.
        '''
        self.tinymovr.set_velocity_setpoint(value * self.encoder_cpr / 60)

    def error(self):
        '''
        Report controller error in human-readable form
        '''
        state = self.tinymovr.state
        error_id = ErrorIDs(state.error)
        print(error_descriptions[error_id] + " (error code: " + str(error_id) + ")")

    @property
    def encoder_cpr(self):
        if self._encoder_cpr < 2048:
            self._encoder_cpr = self.tinymovr.device_info.encoder_cpr
        assert(self._encoder_cpr >= 2048)
        return self._encoder_cpr

    def __dir__(self):
        tm_keys = self.tinymovr.__dir__()
        self_keys = object.__dir__(self)
        self_keys.remove("tinymovr")
        return tm_keys + self_keys
