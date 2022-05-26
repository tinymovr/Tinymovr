"""
Tinymovr Basic Board Tests
Copyright Ioannis Chatzikonstantinou 2020-2022

Tests basic functionality of Tinymovr boards.

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
import statistics as st

import tinymovr
from tinymovr import Tinymovr
from tinymovr.constants import ErrorIDs
from tinymovr.units import get_registry

import unittest
from tests import TMTestCase

ureg = get_registry()
A = ureg.ampere
ticks = ureg.ticks
s = ureg.second


class TestBoard(TMTestCase):
    def test_a_encoder(self):
        """
        Test encoder readings
        """
        pos_estimates = []
        for _ in range(500):
            pos_estimates.append(self.tm.encoder.pos_estimate)
            time.sleep(0.001)
        # apparently the statistics lib works with quantities only
        self.assertLess(st.pstdev(pos_estimates) * ticks, 5 * ticks)

    def test_b_invalid_values(self):
        """
        Test rejection of invalid values for limits and gains
        """
        self.tm.controller.vel_limit = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.vel_limit, 0)
        self.tm.controller.Iq_limit = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.Iq_limit, 0)

        self.tm.controller.vel_gain = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.vel_gain, 0)
        self.tm.controller.pos_gain = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.pos_gain, 0)

        self.tm.controller.vel_integrator.gain = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.vel_integrator.gain, 0)
        self.tm.controller.vel_integrator.deadband = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.vel_integrator.deadband, 0)

        self.tm.controller.vel_integrator.gain = 0  # valid, should be set
        self.assertEqual(self.tm.controller.vel_integrator.gain, 0)
        self.tm.controller.vel_integrator.deadband = 0  # valid, should be set
        self.assertEqual(self.tm.controller.vel_integrator.deadband, 0)

        # Bring it back
        self.tm.reset()

        time.sleep(1)

    def test_c_calibrate(self):
        """
        Test board calibration if not calibrated
        """
        self.check_state(0)
        self.try_calibrate()

    def test_d_position_control(self):
        """
        Test position control
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        for i in range(10):
            self.tm.controller.pos_setpoint = i * 1000 * ticks
            time.sleep(0.2)
            self.assertAlmostEqual(
                i * 1000 * ticks, self.tm.encoder.pos_estimate, delta=1000 * ticks
            )
            time.sleep(0.2)

    def test_e_velocity_control(self):
        """
        Test velocity control
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.velocity_mode()
        self.check_state(2)

        R = 14

        velocity_pairs = []

        for i in range(R):
            target = i * 20000 * ticks / s
            self.tm.controller.vel_setpoint = target
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder.vel_estimate))

        for i in range(R):
            target = (R - i) * 20000 * ticks / s
            self.tm.controller.vel_setpoint = target
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder.vel_estimate))

        for i in range(R):
            target = -i * 20000 * ticks / s
            self.tm.controller.vel_setpoint = target
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder.vel_estimate))

        for i in range(R):
            target = (i - R) * 20000 * ticks / s
            self.tm.controller.vel_setpoint = target
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder.vel_estimate))

        for target, estimate in velocity_pairs:
            self.assertAlmostEqual(target, estimate, delta=30000 * ticks / s)

    def test_f_random_pos_control(self):
        """
        Test random positions
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        for _ in range(10):
            new_pos = random.uniform(-24000, 24000)
            self.tm.controller.pos_setpoint = new_pos * ticks
            time.sleep(0.5)
            self.assertAlmostEqual(
                self.tm.encoder.pos_estimate, self.tm.controller.pos_setpoint
            )

    def test_g_limits(self):
        """
        Test limits
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.velocity_mode()
        self.check_state(2)

        self.tm.controller.vel_limit = 30000
        self.tm.controller.Iq_limit = 0.8
        self.assertAlmostEqual(
            self.tm.controller.vel_limit, 30000 * ticks / s, delta=1 * ticks / s
        )
        self.assertAlmostEqual(self.tm.controller.Iq_limit, 0.8 * A, delta=0.01 * A)

        self.tm.controller.vel_setpoint = 400000 * ticks / s
        time.sleep(0.5)
        self.assertAlmostEqual(
            30000 * ticks / s,
            self.tm.encoder.vel_estimate,
            delta=5000 * ticks / s,
        )
        self.tm.controller.vel_setpoint = -400000 * ticks / s
        time.sleep(0.5)
        self.assertAlmostEqual(
            -30000 * ticks / s,
            self.tm.encoder.vel_estimate,
            delta=5000 * ticks / s,
        )

        self.tm.controller.vel_setpoint = 0
        time.sleep(0.5)

    def test_h_timings(self):
        """
        Test DWT busy/total cycle timings
        """
        self.assertGreater(tm1.cycles.total, 0)
        self.assertGreater(tm1.cycles.busy, 0)
        self.assertLess(tm1.cycles.busy, 3000)

    def test_i_states(self):
        """
        Test state transitions
        """
        self.reset_and_wait()
        # Ensure we're idle
        self.check_state(0)
        # Start calibration
        self.tm.calibrate()
        time.sleep(0.5)
        # Test if idle command works (it should be ignored because we're calibrating)
        self.tm.idle()
        time.sleep(0.5)
        self.assertEqual(self.tm.controller.state, 1)
        # Same for closed loop control command
        self.tm.controller.position_mode()
        time.sleep(0.5)
        self.assertEqual(self.tm.controller.state, 1)
        # Wait for calibration to finish
        self.wait_for_calibration()
        # Now state transitions should work
        self.assertEqual(self.tm.controller.state, 0)
        self.tm.controller.position_mode()
        time.sleep(0.5)
        self.assertEqual(self.tm.controller.state, 2)
        self.tm.controller.idle_mode()

    def test_j_gimbal_mode(self):
        """
        Test gimbal mode
        """
        self.check_state(0)
        self.tm.motor.is_gimbal = True
        self.tm.motor.I_cal = 5.0
        self.tm.motor.R = 0.2
        self.tm.motor.L = 20 * 1e-5
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        for i in range(10):
            self.tm.controller.pos_setpoint = i * 1000 * ticks
            time.sleep(0.2)
            self.assertAlmostEqual(
                i * 1000 * ticks, self.tm.encoder.pos_estimate, delta=1000 * ticks
            )
            time.sleep(0.4)

    def test_k_rotor_offset_and_direction(self):
        """
        Test rotor offset and direction
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        self.assertEqual(self.tm.motor.offset, 0)
        self.assertEqual(self.tm.motor.direction, 1)

        offset = self.tm.encoder.pos_estimate
        self.tm.motor.offset = offset
        self.assertAlmostEqual(self.tm.encoder.pos_estimate, 0, delta=100)

        self.tm.motor.direction = -1
        self.assertAlmostEqual(self.tm.encoder.pos_estimate, 0, delta=100)

        self.tm.controller.pos_setpoint = offset
        time.sleep(0.5)
        self.tm.motor.offset = 0
        self.tm.motor.direction = -1
        self.assertAlmostEqual(self.tm.controller.pos_setpoint, 0, delta=100)

    # def test_l_read_write_endpoints(self):
    #     """
    #     Test read-write endpoints
    #     """
    #     self.check_state(0)
    #     self.try_calibrate()
    #     self.tm.position_control()
    #     self.check_state(2)

    #     self.tm.position_control()
    #     self.tm.set_pos_setpoint(0)

    #     time.sleep(2)

    #     self.tm.velocity_control()

    #     for k in range(10):
    #         values = self.tm.get_set_pos_vel(0, 20000)
    #         time.sleep(1)
    #         self.assertAlmostEqual(
    #             20000 * k * ticks, values.position, delta=2000 * (k + 1) * ticks
    #         )

    def test_m_recalibrate(self):
        """
        Test re-calibration without reset
        """
        self.tm.reset()
        time.sleep(3)
        self.check_state(0)
        self.try_calibrate()
        time.sleep(0.5)
        self.tm.controller.position_mode()
        self.tm.controller.pos_setpoint = 0
        time.sleep(1.5)
        self.tm.controller.pos_setpoint = 50000
        time.sleep(1.5)
        self.tm.controller.idle_mode()
        time.sleep(0.5)
        self.try_calibrate(force=True)
        self.check_state(0)
        self.tm.controller.position_mode()
        pos_estimates = []
        for k in range(50):
            pos_estimates.append(self.tm.encoder.pos_estimate)
            time.sleep(0.05)
        self.tm.controller.idle_mode()
        self.assertLess(st.pstdev(pos_estimates) * ticks, 100 * ticks)


if __name__ == "__main__":
    unittest.main(failfast=True)
