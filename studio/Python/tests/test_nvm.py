"""
Tinymovr Board Config Tests
Copyright Ioannis Chatzikonstantinou 2020-2023

Tests saving and loading Tinymovr configuration to/from NVRAM.

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

import random
import time

from avlos.unit_field import get_registry

import unittest
from tests import TMTestCase

ureg = get_registry()
A = ureg.ampere
tick = ureg.ticks
s = ureg.second
tsleep = 0.30


class TestNVM(TMTestCase):
    def test_a_state_errors(self):
        """
        Test state transitions
        WARNING: This may perform one NVRAM erase cycle.
        """
        if self.tm.motor.calibrated:
            self.tm.erase_config()
            time.sleep(0.2)

        self.reset_and_wait()
        self.check_state(0)
        self.assertEqual(self.tm.motor.calibrated, False)
        self.tm.controller.position_mode()
        self.check_state(0)
        self.reset_and_wait()
        self.check_state(0)
        self.assertEqual(self.tm.motor.calibrated, False)
        self.try_calibrate()
        self.assertEqual(self.tm.motor.calibrated, True)
        self.check_state(0)
        time.sleep(0.2)
        self.tm.controller.position_mode()
        self.check_state(2)
        time.sleep(0.2)
        self.tm.controller.calibrate()
        self.check_state(0)

        self.reset_and_wait()

    def test_b_save_load_config(self):
        """
        Test erasing, saving and loading of config.
        WARNING: This will perform one NVRAM write and two erase cycles.
        """
        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)
        self.assertEqual(self.tm.motor.calibrated, False)
        self.try_calibrate()
        self.assertEqual(self.tm.motor.calibrated, True)
        R = self.tm.motor.R
        L = self.tm.motor.L
        pole_pairs = self.tm.motor.pole_pairs
        self.tm.save_config()
        time.sleep(0.2)
        self.reset_and_wait()
        self.assertEqual(self.tm.motor.calibrated, True)
        self.assertAlmostEqual(R, self.tm.motor.R)
        self.assertAlmostEqual(L, self.tm.motor.L)
        self.assertAlmostEqual(pole_pairs, self.tm.motor.pole_pairs)
        self.tm.erase_config()
        time.sleep(0.2)

    def test_c_parameter_persistence(self):
        """
        Test persisting parameters across config saves.
        WARNING: This will perform one NVRAM write and two erase cycles.
        """
        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)

        self.tm.controller.position.p_gain = 30
        self.tm.controller.velocity.p_gain = 3e-5
        self.tm.controller.velocity.i_gain = 2e-2
        self.tm.controller.velocity.deadband = 100
        self.tm.controller.velocity.limit = 120000
        self.tm.controller.current.Iq_limit = 18
        self.tm.save_config()
        
        time.sleep(0.2)
        self.reset_and_wait()
        self.assertAlmostEqual(self.tm.controller.position.p_gain, 30) #  * 1 / s
        self.assertAlmostEqual(self.tm.controller.velocity.p_gain, 3e-5) #  * A * s / tick
        self.assertAlmostEqual(self.tm.controller.velocity.i_gain, 2e-2) #  * A * s / tick
        self.assertAlmostEqual(self.tm.controller.velocity.deadband, 100 * tick) 
        self.assertAlmostEqual(self.tm.controller.velocity.limit, 120000 * tick / s)
        self.assertAlmostEqual(self.tm.controller.current.Iq_limit, 18 * A)
        self.tm.erase_config()
        time.sleep(0.2)

    def test_d_position_control_w_loaded_config(self):
        """
        Test position control after saving and loading config.
        WARNING: This will perform one NVRAM write and two erase cycles.
        """
        hw_rev = self.tm.hw_revision

        # Define hardware-specific parameters
        if hw_rev > 20:
            tick_range = 8000
            tolerance = 3000
            pos_p_gain = 5
            vel_p_gain = 3e-5
            vel_i_gain = 0
        else:
            tick_range = 24000
            tolerance = 2000
            pos_p_gain = None  # Don't set for older boards
            vel_p_gain = None
            vel_i_gain = None

        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)

        self.try_calibrate()

        # Configure gains before entering position mode
        if pos_p_gain is not None:
            self.tm.controller.position.p_gain = pos_p_gain
        if vel_p_gain is not None:
            self.tm.controller.velocity.p_gain = vel_p_gain
        if vel_i_gain is not None:
            self.tm.controller.velocity.i_gain = vel_i_gain

        self.tm.controller.position_mode()
        self.check_state(2)

        for _ in range(15):
            new_pos = random.uniform(-tick_range, tick_range)
            self.tm.controller.position.setpoint = new_pos * tick
            time.sleep(tsleep)
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.position_estimate,
                self.tm.controller.position.setpoint,
                delta=tolerance * tick,
            )

        self.tm.controller.idle()

        time.sleep(0.1)

        self.tm.save_config()

        time.sleep(0.2)

        self.reset_and_wait()

        self.tm.controller.position_mode()
        self.check_state(2)

        for _ in range(15):
            new_pos = random.uniform(-tick_range, tick_range)
            self.tm.controller.position.setpoint = new_pos * tick
            time.sleep(tsleep)
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.position_estimate,
                self.tm.controller.position.setpoint,
                delta=tolerance * tick,
            )

        self.tm.erase_config()
        time.sleep(0.2)

    def test_e_save_settings_and_reset(self):
        """
        Test case for issue reported by @Phosfor on Discord:
        1. Configure current limits etc.
        2. Calibrate the controller
        3. Make sure everything is working (i.e. enter current/velocity mode)
        4. Save the configuration
        5. Reset the tinymovr; all settings are still correct and the tinymovr reports that it is calibrated
        6. Try to enter current/velocity mode again; the tinymovr says it is in the correct mode and in the control state, but the motor is not turning
        """
        hw_rev = self.tm.hw_revision

        # Define hardware-specific parameters
        if hw_rev > 20:
            iq_limit = 10.0
            velocity_limit = 80000
            vel_i_gain = 0
            vel_p_gain = 3e-5
            pos_p_gain = 5
            vel_deadband = 50
            max_vel = 60000
            max_accel = 300000
            max_decel = 300000
            velocity_range = 20000
        else:
            iq_limit = 15.0
            velocity_limit = 200000
            vel_i_gain = 1e-5
            vel_p_gain = 1.6e-4
            pos_p_gain = None  # Don't set for older boards
            vel_deadband = 50
            max_vel = 110000
            max_accel = 500000
            max_decel = 500000
            velocity_range = 50000

        max_delta = 5000

        # Step 1: Configure current limits etc.
        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)

        self.tm.controller.current.Iq_limit = iq_limit * A
        self.tm.controller.velocity.limit = velocity_limit * tick / s
        self.tm.controller.velocity.i_gain = vel_i_gain
        self.tm.controller.velocity.p_gain = vel_p_gain
        self.tm.controller.velocity.deadband = vel_deadband * tick
        if pos_p_gain is not None:
            self.tm.controller.position.p_gain = pos_p_gain
        self.tm.traj_planner.max_vel = max_vel * tick / s
        self.tm.traj_planner.max_accel = max_accel * tick / s
        self.tm.traj_planner.max_decel = max_decel * tick / (s * s)

        # Step 2: Calibrate the controller
        self.try_calibrate()
        self.assertEqual(self.tm.motor.calibrated, True)

        # Step 3: Make sure everything is working (i.e. enter current/velocity mode)
        self.tm.controller.velocity_mode()
        self.check_state(2)

        for _ in range(5):
            new_velocity = random.uniform(-velocity_range, velocity_range)
            self.tm.controller.velocity.setpoint = new_velocity * tick / s
            time.sleep(tsleep)
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.velocity_estimate,
                self.tm.controller.velocity.setpoint,
                delta=max_delta * tick / s,
            )

        self.tm.controller.idle()
        time.sleep(0.1)

        # Step 4: Save the configuration
        self.tm.save_config()
        time.sleep(0.2)

        # Step 5: Reset the tinymovr; all settings are still correct and the tinymovr reports that it is calibrated
        self.reset_and_wait()
        self.assertEqual(self.tm.motor.calibrated, True)
        self.assertAlmostEqual(self.tm.controller.current.Iq_limit, iq_limit * A)
        self.assertAlmostEqual(self.tm.controller.velocity.limit, velocity_limit * tick / s)
        self.assertAlmostEqual(self.tm.controller.velocity.i_gain, vel_i_gain)
        self.assertAlmostEqual(self.tm.controller.velocity.p_gain, vel_p_gain)
        self.assertAlmostEqual(self.tm.controller.velocity.deadband, vel_deadband * tick)
        self.assertAlmostEqual(self.tm.traj_planner.max_vel, max_vel * tick / s)
        self.assertAlmostEqual(self.tm.traj_planner.max_accel, max_accel * tick / s)
        self.assertAlmostEqual(self.tm.traj_planner.max_decel, max_decel * tick / (s * s))

        # Step 6: Try to enter current/velocity mode again; the tinymovr says it is in the correct mode and in the control state, but the motor is not turning (and I don't hear any switching noise that I usually hear when the tinymovr is in the active state)
        self.tm.controller.velocity_mode()
        self.check_state(2)

        for _ in range(5):
            new_velocity = random.uniform(-velocity_range, velocity_range)
            self.tm.controller.velocity.setpoint = new_velocity * tick / s
            time.sleep(tsleep)
            # Here we expect the motor not to turn, so we check for lack of movement
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.velocity_estimate,
                self.tm.controller.velocity.setpoint,
                delta=max_delta * tick / s,
            )

        self.tm.controller.idle()
        time.sleep(0.1)

        self.tm.erase_config()
        time.sleep(0.2)



if __name__ == "__main__":
    unittest.main()
