'''
This unit test suite tests functionality of the
Tinymovr Studio using a simulated Tinymovr
device, which is suitable for unit testing.
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
        self.assertLess(st.pstdev(pos_estimates) * ticks, 10 * ticks)

    def test_b_calibrate(self):
        '''
        Test board calibration if not calibrated
        '''
        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 0)

        motor_info = self.tm.motor_info

        if motor_info.calibrated == 0:
            self.tm.calibrate()
            for _ in range(100):
                if self.tm.state.state == 0:
                    break
                time.sleep(0.5)
            motor_info = self.tm.motor_info
            self.assertEqual(motor_info.calibrated, 1)

    def test_c_position_control(self):
        '''
        Test position control
        '''
        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 0)

        self.tm.position_control()

        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 2)

        for i in range(10):
            self.tm.set_pos_setpoint(i*1000*ticks)
            time.sleep(0.2)
            self.assertAlmostEqual(i*1000*ticks, self.tm.encoder_estimates.position, delta=1000*ticks)
            time.sleep(0.4)

    def test_d_velocity_control(self):
        '''
        Test velocity control
        '''
        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 0)

        self.tm.velocity_control()

        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 2)

        R = 14

        for i in range(R):
            self.tm.set_vel_setpoint(i*20000*ticks/s)
            time.sleep(0.2)
            self.assertAlmostEqual(i*20000*ticks/s, self.tm.encoder_estimates.velocity, delta=30000*ticks/s)
            time.sleep(0.3)

        for i in range(R):
            self.tm.set_vel_setpoint((R-i)*20000*ticks/s)
            time.sleep(0.2)
            self.assertAlmostEqual((R-i)*20000*ticks/s, self.tm.encoder_estimates.velocity, delta=30000*ticks/s)
            time.sleep(0.3)

        for i in range(R):
            self.tm.set_vel_setpoint(-i*20000*ticks/s)
            time.sleep(0.2)
            self.assertAlmostEqual(-i*20000*ticks/s, self.tm.encoder_estimates.velocity, delta=30000*ticks/s)
            time.sleep(0.3)

        for i in range(R):
            self.tm.set_vel_setpoint((i-R)*20000*ticks/s)
            time.sleep(0.2)
            self.assertAlmostEqual((i-R)*20000*ticks/s, self.tm.encoder_estimates.velocity, delta=30000*ticks/s)
            time.sleep(0.3)

    def test_e_random_pos_control(self):
        '''
        Test random positions
        '''
        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 0)

        self.tm.position_control()

        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 2)

        for i in range(10):
            new_pos = random.uniform(-24000, 24000)
            self.tm.set_pos_setpoint(new_pos * ticks)
            time.sleep(0.5)

    def test_f_limits(self):
        '''
        Test limits
        '''
        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 0)

        limits = self.tm.limits
        self.tm.set_limits(30000, 0.8)
        new_limits = self.tm.limits
        self.assertEqual(new_limits.velocity, 30000 * ticks/s)
        self.assertEqual(new_limits.current, 0.8 * A)

        self.tm.velocity_control()

        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 1)

        self.tm.set_vel_setpoint(400000 * ticks/s)
        time.sleep(0.5)
        self.assertAlmostEqual(30000 * ticks/s, self.tm.encoder_estimates.velocity, delta=5000 * ticks/s)

        self.tm.set_vel_setpoint(0)

        time.sleep(0.5)


    def tearDown(self):
        self.tm.idle()

    @classmethod
    def tearDownClass(cls):
        cls.tm.reset()


if __name__ == '__main__':
    unittest.main()
