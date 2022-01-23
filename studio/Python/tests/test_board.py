'''
This unit test suite tests functionality
of Tinymovr boards.
'''
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
        '''
        Test encoder readings
        '''
        pos_estimates = []
        for _ in range(500):
            pos_estimates.append(self.tm.encoder_estimates.position)
            time.sleep(0.001)
        # apparently the statistics lib works with quantities only
        self.assertLess(st.pstdev(pos_estimates) * ticks, 5 * ticks)

    def test_b_invalid_values(self):
        '''
        Test rejection of invalid values for limits and gains
        '''
        limits = self.tm.limits
        self.tm.set_limits(-10, -10) # invalid, should not be set
        self.assertEqual(limits, self.tm.limits)

        gains = self.tm.gains
        self.tm.set_gains(-10, -10) # invalid, should not be set
        self.assertEqual(gains, self.tm.gains)

        int_gains = self.tm.integrator_gains
        self.tm.set_integrator_gains(-10) # invalid, should not be set
        self.assertEqual(int_gains, self.tm.integrator_gains)

        # Zero vel integrator values should be allowed
        self.tm.set_integrator_gains(0) # valid, should be set
        self.assertEqual(self.tm.integrator_gains, 0)

        # Bring it back
        self.tm.set_integrator_gains(int_gains)

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
        self.tm.position_control()
        self.check_state(2)

        for i in range(10):
            self.tm.set_pos_setpoint(i*1000*ticks)
            time.sleep(0.2)
            self.assertAlmostEqual(i*1000*ticks, self.tm.encoder_estimates.position, delta=1000*ticks)
            time.sleep(0.4)

    def test_e_velocity_control(self):
        '''
        Test velocity control
        '''
        self.check_state(0)
        self.try_calibrate()
        self.tm.velocity_control()
        self.check_state(2)

        R = 14

        velocity_pairs = []

        for i in range(R):
            target = i*20000*ticks/s
            self.tm.set_vel_setpoint(target)
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder_estimates.velocity))
            time.sleep(0.3)

        for i in range(R):
            target = (R-i)*20000*ticks/s
            self.tm.set_vel_setpoint(target)
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder_estimates.velocity))
            time.sleep(0.3)

        for i in range(R):
            target = -i*20000*ticks/s
            self.tm.set_vel_setpoint(target)
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder_estimates.velocity))
            time.sleep(0.3)

        for i in range(R):
            target = (i-R)*20000*ticks/s
            self.tm.set_vel_setpoint(target)
            time.sleep(0.2)
            velocity_pairs.append((target, self.tm.encoder_estimates.velocity))
            time.sleep(0.3)

        for target, estimate in velocity_pairs:
            self.assertAlmostEqual(target, estimate, delta=30000*ticks/s)

    def test_f_random_pos_control(self):
        '''
        Test random positions
        '''
        self.check_state(0)
        self.try_calibrate()
        self.tm.position_control()
        self.check_state(2)

        for _ in range(10):
            new_pos = random.uniform(-24000, 24000)
            self.tm.set_pos_setpoint(new_pos * ticks)
            time.sleep(0.5)

    def test_g_limits(self):
        '''
        Test limits
        '''
        self.check_state(0)
        self.try_calibrate()
        self.tm.velocity_control()
        self.check_state(2)

        self.tm.set_limits(30000, 0.8)
        new_limits = self.tm.limits
        self.assertAlmostEqual(new_limits.velocity, 30000 * ticks/s, delta=1*ticks/s)
        self.assertAlmostEqual(new_limits.current, 0.8 * A, delta=0.01 * A)

        self.tm.set_vel_setpoint(400000 * ticks/s)
        time.sleep(0.5)
        self.assertAlmostEqual(30000 * ticks/s, self.tm.encoder_estimates.velocity, delta=5000 * ticks/s)
        self.tm.set_vel_setpoint(-400000 * ticks/s)
        time.sleep(0.5)
        self.assertAlmostEqual(-30000 * ticks/s, self.tm.encoder_estimates.velocity, delta=5000 * ticks/s)

        self.tm.set_vel_setpoint(0)

        time.sleep(0.5)

    def test_h_timings(self):
        '''
        Test DWT busy/total cycle timings
        '''
        tim = self.tm.timings
        self.assertGreater(tim.total, 0)
        self.assertGreater(tim.busy, 0)
        self.assertLess(tim.busy, 3000)

    def test_i_states(self):
        '''
        Test state transitions
        '''
        self.reset_and_wait()
        # Ensure we're idle
        self.check_state(0)
        # Start calibration
        self.tm.calibrate()
        time.sleep(0.5)
        # Test if idle command works (it should be ignored because we're calibrating)
        self.tm.idle()
        time.sleep(0.5)
        self.assertEqual(self.tm.state.state, 1)
        # Same for closed loop control command
        self.tm.position_control()
        time.sleep(0.5)
        self.assertEqual(self.tm.state.state, 1)
        # Wait for calibration to finish
        self.wait_for_calibration()
        # Now state transitions should work
        self.assertEqual(self.tm.state.state, 0)
        self.tm.position_control()
        time.sleep(0.5)
        self.assertEqual(self.tm.state.state, 2)
        self.tm.idle()

    def test_j_gimbal_mode(self):
        '''
        Test gimbal mode
        '''
        self.check_state(0)
        self.tm.set_motor_config(flags=1, pole_pairs=1, I_cal=5.0) # fw will ignore pole pair value of 1
        self.tm.set_motor_RL(R=0.2, L=20*1e-5)
        self.try_calibrate()
        self.tm.position_control()
        self.check_state(2)

        for i in range(10):
            self.tm.set_pos_setpoint(i*1000*ticks)
            time.sleep(0.2)
            self.assertAlmostEqual(i*1000*ticks, self.tm.encoder_estimates.position, delta=1000*ticks)
            time.sleep(0.4)

    def test_k_rotor_offset_and_direction(self):
        '''
        Test rotor offset and direction
        '''
        self.check_state(0)
        self.try_calibrate()
        self.tm.position_control()
        self.check_state(2)

        self.assertEqual(self.tm.offset_dir.offset, 0)
        self.assertEqual(self.tm.offset_dir.direction, 1)

        offset = self.tm.encoder_estimates.position
        self.tm.set_offset_dir(offset, 1)
        self.assertAlmostEqual(self.tm.encoder_estimates.position, 0, delta=100)

        self.tm.set_offset_dir(offset, -1)
        self.assertAlmostEqual(self.tm.encoder_estimates.position, 0, delta=100)

        self.tm.set_pos_setpoint(offset)
        time.sleep(0.5)
        self.tm.set_offset_dir(0, 1)
        self.assertAlmostEqual(self.tm.encoder_estimates.position, 0, delta=100)

    def test_l_read_write_endpoints(self):
        '''
        Test read-write endpoints
        '''
        self.check_state(0)
        self.try_calibrate()
        self.tm.position_control()
        self.check_state(2)

        self.tm.position_control()
        self.tm.set_pos_setpoint(0)

        time.sleep(2)

        self.tm.velocity_control()

        for k in range(10):
            values = self.tm.get_set_pos_vel(0, 20000)
            time.sleep(1)
            self.assertAlmostEqual(20000*k*ticks, values.position, delta=2000*(k+1)*ticks)



if __name__ == '__main__':
    unittest.main(failfast=True)
