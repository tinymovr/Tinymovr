'''
This unit test suite tests functionality
of Tinymovr boards.
'''
import random
import time
import can
import statistics as st

import tinymovr
from tinymovr import Tinymovr, ErrorIDs
from tinymovr.iface import IFace
from tinymovr.iface.can import CAN, guess_channel
from tinymovr.units import get_registry

import unittest

ureg = get_registry()
A = ureg.ampere
ticks = ureg.ticks
s = ureg.second

bustype = "slcan"


class TestBoard(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        channel = guess_channel(bustype_hint=bustype)
        can_bus: can.Bus = can.Bus(bustype=bustype, channel=channel)
        iface: IFace = CAN(can_bus)
        cls.tm = Tinymovr(node_id=1, iface=iface)
        cls.tm.reset()
        time.sleep(0.2)

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

    def test_b_calibrate(self):
        '''
        Test board calibration if not calibrated
        '''
        self.check_state(0)
        self.try_calibrate()

    def test_c_position_control(self):
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

    def test_d_velocity_control(self):
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

    def test_e_random_pos_control(self):
        '''
        Test random positions
        '''
        self.check_state(0)
        self.try_calibrate()
        self.tm.position_control()
        self.check_state(2)

        for i in range(10):
            new_pos = random.uniform(-24000, 24000)
            self.tm.set_pos_setpoint(new_pos * ticks)
            time.sleep(0.5)

    def test_f_limits(self):
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

    def tearDown(self):
        self.tm.idle()

    @classmethod
    def tearDownClass(cls):
        cls.tm.reset()

    def try_calibrate(self):
        motor_info = self.tm.motor_info
        if motor_info.calibrated == 0:
            self.tm.calibrate()
            for _ in range(100):
                if self.tm.state.state == 0:
                    break
                time.sleep(0.5)
            motor_info = self.tm.motor_info
            self.assertEqual(motor_info.calibrated, 1)

    def check_state(self, target_state, target_error=ErrorIDs.NoError):
        state = self.tm.state
        self.assertEqual(state.error, target_error)
        self.assertEqual(state.state, target_state)


if __name__ == '__main__':
    unittest.main()
