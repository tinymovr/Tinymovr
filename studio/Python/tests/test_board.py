'''
This unit test suite tests functionality of the
Tinymovr Studio using a simulated Tinymovr
device, which is suitable for unit testing.
'''
import random
import time
import can

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

    def setUp(self):
        channel = guess_channel(bustype_hint=bustype)
        can_bus: can.Bus = can.Bus(bustype=bustype, channel=channel)
        iface: IFace = CAN(can_bus)
        self.tm = Tinymovr(node_id=1, iface=iface)

    def test_a_calibrate(self):
        '''
        Suite of board tests for validation
        '''
        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 0)

        motor_info = self.tm.motor_info

        if motor_info.calibrated == 0:
            self.tm.calibrate()
            for i in range(100):
                if self.tm.state.state == 0:
                    break
                time.sleep(0.5)
            motor_info = self.tm.motor_info
            self.assertEqual(motor_info.calibrated, 1)

    def test_b_position_control(self):
        '''
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

    def test_c_velocity_control(self):
        '''
        '''
        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 0)

        self.tm.velocity_control()

        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 2)

        for i in range(10):
            self.tm.set_vel_setpoint(i*20000*ticks/s)
            time.sleep(0.2)
            self.assertAlmostEqual(i*20000*ticks/s, self.tm.encoder_estimates.velocity, delta=30000*ticks/s)
            time.sleep(0.8)

        for i in range(10):
            self.tm.set_vel_setpoint((10-i)*20000*ticks/s)
            time.sleep(0.2)
            self.assertAlmostEqual((10-i)*20000*ticks/s, self.tm.encoder_estimates.velocity, delta=30000*ticks/s)
            time.sleep(0.8)

    def tearDown(self):
        self.tm.idle()


if __name__ == '__main__':
    unittest.main()