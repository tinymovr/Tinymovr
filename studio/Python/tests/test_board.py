"""
This unit test suite tests basic functionality
of a Tinymovr board.
"""
import random
import time
import statistics as st

from tests import TMTestCase
import unittest


class TestSimulation(TMTestCase):

    def test_a_encoder(self):
        '''
        Test encoder readings
        '''
        pos_estimates = []
        for _ in range(500):
            pos_estimates.append(self.tm.encoder.pos_estimate)
            time.sleep(0.001)
        self.assertLess(st.pstdev(pos_estimates), 5)

    def test_b_invalid_values(self):
        '''
        Test rejection of invalid values for limits and gains
        '''
        limits = self.tm.controller.vel_limit, self.tm.controller.Iq_limit
        self.tm.controller.vel_limit, self.tm.controller.Iq_limit = -10, -10 # invalid, should not be set
        self.assertEqual(limits, (self.tm.controller.vel_limit, self.tm.controller.Iq_limit))

        gains = self.tm.controller.pos_gain, self.tm.controller.vel_gain
        self.tm.controller.pos_gain, self.tm.controller.vel_gain = -10, -10 # invalid, should not be set
        self.assertEqual(gains, self.tm.controller.pos_gain, self.tm.controller.vel_gain)

        int_gain = self.tm.controller.vel_integrator_gain
        self.tm.controller.vel_integrator_gain = -10 # invalid, should not be set
        self.assertEqual(int_gain, self.tm.controller.vel_integrator_gain)

        # Zero vel integrator values should be allowed
        self.tm.controller.vel_integrator_gain = 0 # valid, should be set
        self.assertEqual(self.tm.controller.vel_integrator_gain, 0)

        # Bring it back
        self.tm.controller.vel_integrator_gain = int_gain

    def test_c_calibrate(self):
        '''
        Test board calibration if not calibrated
        '''
        self.check_state(0)
        self.try_calibrate()

    def test_d_position_control(self):
        '''
        Test position control
        '''
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.state = 2
        self.tm.controller.mode = 2
        self.check_state(2)

        for i in range(10):
            self.tm.controller.pos_setpoint = i*1000
            time.sleep(0.2)
            self.assertAlmostEqual(i*1000, self.tm.encoder.pos_estimate, delta=1000)
            time.sleep(0.4)

    def test_e_velocity_control(self):
        '''
        Test velocity control
        '''
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.state = 2
        self.tm.controller.mode = 1
        self.check_state(2)

        R = 14

        velocity_pairs = []
        for i in range(R):
            target = i*20000
            self.tm.controller.vel_setpoint = target
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder.vel_estimate))
            time.sleep(0.1)

        for i in range(R):
            target = (R-i)*20000
            self.tm.controller.vel_setpoint = target
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder.vel_estimate))
            time.sleep(0.1)

        for i in range(R):
            target = -i*20000
            self.tm.controller.vel_setpoint = target
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder.vel_estimate))
            time.sleep(0.1)

        for i in range(R):
            target = (i-R)*20000
            self.tm.controller.vel_setpoint = target
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder.vel_estimate))
            time.sleep(0.1)

        for target, estimate in velocity_pairs:
            self.assertAlmostEqual(target, estimate, delta=30000)

    def test_f_random_pos_control(self):
        '''
        Test random rotor positions with varying intervals
        '''
        self.random_pos_control(0.5)
        self.random_pos_control(0.1)

    def random_pos_control(self, interval):
        '''
        Set random rotor positions
        '''
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.state = 2
        self.tm.controller.mode = 2
        self.check_state(2)

        for _ in range(10):
            self.tm.controller.pos_setpoint = random.uniform(-24000, 24000)
            time.sleep(interval)

    def test_h_timings(self):
        '''
        Test DWT busy/total cycle timings
        '''
        self.assertLess(self.tm.busy, 3600)

    def test_i_states(self):
        '''
        Test state transitions
        '''
        self.tm.reset()
        time.sleep(0.5)
        # Ensure we're idle
        self.check_state(0)
        # Start calibration
        self.tm.controller.state = 1
        time.sleep(0.5)
        # Test if idle command works (it should be ignored because we're calibrating)
        self.tm.controller.state = 0
        time.sleep(0.5)
        self.assertEqual(self.tm.controller.state, 1)
        # Same for closed loop control command
        self.tm.controller.state = 2
        time.sleep(0.5)
        self.assertEqual(self.tm.controller.state, 1)
        # Wait for calibration to finish
        self.wait_for_calibration()
        # Now state transitions should work
        self.assertEqual(self.tm.controller.state, 0)
        self.tm.controller.state = 2
        self.tm.controller.mode = 2
        time.sleep(0.5)
        self.assertEqual(self.tm.controller.state, 2)
        self.tm.controller.state = 0

    def test_j_gimbal_mode(self):
        '''
        Test gimbal mode
        '''
        self.check_state(0)
        self.tm.motor.gimbal = True
        self.motor.R = 0.22
        self.motor.L = 8 * 1e-5
        self.motor.I_cal = 5
        self.try_calibrate()
        self.tm.controller.state = 2
        self.tm.controller.mode = 2
        self.check_state(2)

        for i in range(10):
            self.tm.controller.pos_setpoint = i*1000
            time.sleep(0.2)
            self.assertAlmostEqual(i*1000, self.tm.encoder_estimates.position, delta=1000)
            time.sleep(0.4)


if __name__ == "__main__":
    unittest.main()
