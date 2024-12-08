"""
Tinymovr Basic Board Tests
Copyright Ioannis Chatzikonstantinou 2020-2023

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
import numpy as np
import statistics as st
from avlos.unit_field import get_registry

import unittest
import pytest
from tests import TMTestCase

ureg = get_registry()
A = ureg.ampere
ticks = ureg.ticks
s = ureg.second
tsleep = 0.20


class TestBoard(TMTestCase):

    @pytest.mark.hitl_default
    @pytest.mark.hitl_mini
    def test_a_sensor_readings(self):
        """
        Test sensor readings
        """
        pos_estimates = []
        for _ in range(500):
            pos_estimates.append(self.tm.sensors.user_frame.position_estimate)
            time.sleep(0.001)
        # apparently the statistics lib works with quantities only
        self.assertLess(st.pstdev(pos_estimates) * ticks, 5 * ticks)

    @pytest.mark.hitl_default
    @pytest.mark.hitl_mini
    def test_b_invalid_values(self):
        """
        Test rejection of invalid values for limits and gains
        """
        self.tm.controller.velocity.limit = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.velocity.limit, 0)
        self.tm.controller.current.Iq_limit = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.current.Iq_limit, 0)

        self.tm.controller.velocity.p_gain = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.velocity.p_gain, 0)
        self.tm.controller.position.p_gain = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.position.p_gain, 0)

        self.tm.controller.velocity.i_gain = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.velocity.i_gain, 0)
        self.tm.controller.velocity.deadband = -10  # invalid, should not be set
        self.assertGreater(self.tm.controller.velocity.deadband, 0)

        self.tm.controller.velocity.i_gain = 0  # valid, should be set
        self.assertEqual(self.tm.controller.velocity.i_gain, 0)
        self.tm.controller.velocity.deadband = 0  # valid, should be set
        self.assertEqual(self.tm.controller.velocity.deadband, 0)

        # Bring it back
        self.tm.reset()

        time.sleep(2)

    @pytest.mark.hitl_default
    @pytest.mark.hitl_mini
    def test_c_calibrate(self):
        """
        Test board calibration if not calibrated
        """
        self.check_state(0)
        self.try_calibrate()

    @pytest.mark.eol
    @pytest.mark.hitl_default
    @pytest.mark.hitl_mini
    def test_d_position_control(self):
        """
        Test position control
        """
        hw_rev = self.tm.hw_revision
        self.check_state(0)
        self.tm.controller.current.Iq_limit = 5
        self.try_calibrate()
        if hw_rev > 20:
            self.tm.controller.position.p_gain = 5
            self.tm.controller.velocity.p_gain = 2e-5
            self.tm.controller.velocity.i_gain = 0

        self.tm.controller.position_mode()
        self.check_state(2)

        self.tm.controller.position.setpoint = 0
        time.sleep(0.4)

        for i in range(5):
            self.tm.controller.position.setpoint = i * 3000 * ticks
            if hw_rev > 20:
                time.sleep(0.50)
            else:
                time.sleep(0.20)
            self.assertAlmostEqual(
                i * 3000 * ticks, self.tm.sensors.user_frame.position_estimate, delta=1000 * ticks
            )

    @pytest.mark.hitl_default
    @pytest.mark.hitl_mini
    def test_e_velocity_control(self):
        """
        Test velocity control
        """
        hw_rev = self.tm.hw_revision
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.velocity_mode()
        self.check_state(2)

        R = 15

        if hw_rev > 20:
            multiplier = 4000 * ticks / s
            max_delta = 15000 * ticks / s
        else:
            multiplier = 20000 * ticks / s
            max_delta = 30000 * ticks / s

        velocity_pairs = []

        for i in range(R):
            target = i * multiplier
            self.tm.controller.velocity.setpoint = target
            time.sleep(tsleep)
            velocity_pairs.append((target, self.tm.sensors.user_frame.velocity_estimate))

        for i in range(R):
            target = (R - i) * multiplier
            self.tm.controller.velocity.setpoint = target
            time.sleep(tsleep)
            velocity_pairs.append((target, self.tm.sensors.user_frame.velocity_estimate))

        for i in range(R):
            target = -i * multiplier
            self.tm.controller.velocity.setpoint = target
            time.sleep(tsleep)
            velocity_pairs.append((target, self.tm.sensors.user_frame.velocity_estimate))

        for i in range(R):
            target = (i - R) * multiplier
            self.tm.controller.velocity.setpoint = target
            time.sleep(tsleep)
            velocity_pairs.append((target, self.tm.sensors.user_frame.velocity_estimate))

        for target, estimate in velocity_pairs:
            self.assertAlmostEqual(target, estimate, delta=max_delta)

    @pytest.mark.hitl_default
    @pytest.mark.hitl_mini
    def test_f_random_pos_control(self):
        """
        Test random positions
        """
        hw_rev = self.tm.hw_revision
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        if hw_rev > 20:
            self.tm.controller.velocity.limit = 100000 * ticks / s
            tick_range = 10000
        else:
            self.tm.controller.velocity.limit = 200000 * ticks / s
            tick_range = 24000

        for _ in range(15):
            new_pos = random.uniform(-tick_range, tick_range)
            self.tm.controller.position.setpoint = new_pos * ticks
            
            time.sleep(0.35)
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.position_estimate,
                self.tm.controller.position.setpoint,
                delta=2000 * ticks,
            )

    @pytest.mark.hitl_default
    def test_g_limits(self):
        """
        Test limits
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.velocity_mode()
        self.check_state(2)

        self.tm.controller.velocity.limit = 30000
        self.tm.controller.current.Iq_limit = 0.8
        self.assertAlmostEqual(
            self.tm.controller.velocity.limit, 30000 * ticks / s, delta=1 * ticks / s
        )
        self.assertAlmostEqual(
            self.tm.controller.current.Iq_limit, 0.8 * A, delta=0.01 * A
        )

        self.tm.controller.velocity.setpoint = 400000 * ticks / s
        time.sleep(0.5)
        self.assertAlmostEqual(
            30000 * ticks / s,
            self.tm.sensors.user_frame.velocity_estimate,
            delta=5000 * ticks / s,
        )
        self.tm.controller.velocity.setpoint = -400000 * ticks / s
        time.sleep(0.5)
        self.assertAlmostEqual(
            -30000 * ticks / s,
            self.tm.sensors.user_frame.velocity_estimate,
            delta=5000 * ticks / s,
        )

        self.tm.controller.velocity.setpoint = 0
        time.sleep(0.5)

    @pytest.mark.hitl_default
    def test_h_timings(self):
        """
        Test DWT busy/total cycle timings
        """
        if (self.tm.scheduler.load == 0 or self.tm.scheduler.load > 7000):
            self.skipTest("Invalid timing values. Skipping test.")
        self.assertLess(self.tm.scheduler.load, 4000)

    @pytest.mark.hitl_default
    def test_i_states(self):
        """
        Test state transitions
        """
        self.reset_and_wait()
        # Ensure we're idle
        self.check_state(0)
        # Start calibration
        self.tm.controller.calibrate()
        time.sleep(0.5)
        # Test if idle command works (it should be ignored because we're calibrating)
        self.tm.controller.idle()
        time.sleep(0.1)
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
        self.tm.controller.idle()

    @pytest.mark.hitl_default
    def test_j_gimbal_mode(self):
        """
        Test gimbal mode
        """
        if self.tm.hw_revision > 20 and self.tm.hw_revision < 30:
            self.skipTest("Gimbal test not relevant to Tinymovr M5.x. Skipping test.")
        self.reset_and_wait()
        # Ensure we're idle
        self.check_state(0)
        self.tm.motor.type = 1  # gimbal
        self.tm.motor.R = 0.2
        self.tm.motor.L = 20 * 1e-5
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        for i in range(5):
            self.tm.controller.position.setpoint = i * 2000 * ticks
            time.sleep(0.3)
            self.assertAlmostEqual(
                i * 2000 * ticks, self.tm.sensors.user_frame.position_estimate, delta=2000 * ticks
            )

    @pytest.mark.hitl_default
    def test_k_rotor_offset_and_multiplier(self):
        """
        Test rotor offset and multiplier
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        self.assertEqual(self.tm.sensors.user_frame.offset, 0)
        self.assertEqual(self.tm.sensors.user_frame.multiplier, 1)

        offset = self.tm.sensors.user_frame.position_estimate
        self.tm.sensors.user_frame.offset = offset
        self.assertAlmostEqual(self.tm.sensors.user_frame.position_estimate, 0, delta=100)

        self.tm.sensors.user_frame.multiplier = -1
        self.assertAlmostEqual(self.tm.sensors.user_frame.position_estimate, 0, delta=100)

        self.tm.controller.position.setpoint = offset
        time.sleep(0.5)
        self.tm.sensors.user_frame.offset = 0
        self.tm.sensors.user_frame.multiplier = -1
        self.assertAlmostEqual(self.tm.controller.position.setpoint, 0, delta=100)

    # def test_l_read_write_endpoints(self):
    #     """
    #     Test read-write endpoints
    #     """
    #     self.check_state(0)
    #     self.try_calibrate()
    #     self.tm.position_control()
    #     self.check_state(2)

    #     self.tm.position_control()
    #     self.tm.set_position.setpoint(0)

    #     time.sleep(2)

    #     self.tm.velocity_control()

    #     for k in range(10):
    #         values = self.tm.get_set_pos_vel(0, 20000)
    #         time.sleep(1)
    #         self.assertAlmostEqual(
    #             20000 * k * ticks, values.position, delta=2000 * (k + 1) * ticks
    #         )

    @pytest.mark.hitl_default
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
        self.tm.controller.position.setpoint = 0
        time.sleep(1.5)
        self.tm.controller.position.setpoint = 50000
        time.sleep(1.5)
        self.tm.controller.idle()
        time.sleep(0.5)
        self.try_calibrate(force=True)
        self.check_state(0)
        self.tm.controller.position_mode()
        pos_estimates = []
        for k in range(50):
            pos_estimates.append(self.tm.sensors.user_frame.position_estimate)
            time.sleep(0.05)
        self.tm.controller.idle()
        self.assertLess(st.pstdev(pos_estimates) * ticks, 100 * ticks)

    @pytest.mark.hitl_default
    def test_n_velocity_ramp(self):
        """
        Test velocity ramp
        """
        ramp_value = 1
        interval = 0.05
        frequency = 20000

        self.reset_and_wait()
        # Ensure we're idle
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.velocity.increment = ramp_value
        vel_estimates = []
        t_points = []
        self.tm.controller.velocity_mode()
        self.tm.controller.velocity.setpoint = 200000
        for k in range(100):
            vel_estimates.append(self.tm.sensors.user_frame.velocity_estimate.magnitude)
            t_points.append(k * interval)
            time.sleep(interval)
        self.tm.controller.idle()
        a, _ = np.polyfit(t_points, vel_estimates, 1)
        self.assertAlmostEqual(a, ramp_value * frequency, delta=5000)

    @pytest.mark.hitl_default
    def test_o_composite_set_get(self):
        """
        Test composite setter/getter function
        """
        self.reset_and_wait()
        # Ensure we're idle
        self.check_state(0)
        self.try_calibrate()
        pos_estimate_ref = self.tm.sensors.user_frame.position_estimate
        pos_estimate_comp = self.tm.controller.set_pos_vel_setpoints(0, 0) * ticks
        self.assertAlmostEqual(pos_estimate_ref, pos_estimate_comp, delta=200*ticks)

    @pytest.mark.hitl_default
    def test_p_flux_braking(self):
        """
        Test flux braking
        """
        self.skipTest("Temporarily skipping flux braking test.")
        self.reset_and_wait()
        # Ensure we're idle
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.current.max_Ibrake = 0
        self.tm.controller.velocity_mode()
        for v_set in [-250000, 250000]:
            self.tm.controller.velocity.setpoint = v_set
            time.sleep(0.4)
            self.tm.controller.velocity.setpoint = 0
            I_brake_vals = []
            for _ in range(200):
                I_brake_vals.append(self.tm.Ibus)
                time.sleep(0.001)
            time.sleep(0.2)
            self.assertLess(min(I_brake_vals), -0.12 * A)
        self.tm.controller.current.max_Ibrake = 10
        for v_set in [-250000, 250000]:
            self.tm.controller.velocity.setpoint = v_set
            time.sleep(0.4)
            self.tm.controller.velocity.setpoint = 0
            I_brake_vals = []
            for _ in range(200):
                I_brake_vals.append(self.tm.Ibus)
                time.sleep(0.001)
            time.sleep(0.2)
            self.assertGreater(min(I_brake_vals), -0.12 * A)
        self.tm.controller.current.max_Ibrake = 0
        self.tm.controller.idle()
        time.sleep(0.4)


if __name__ == "__main__":
    unittest.main(failfast=True)
