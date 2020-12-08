'''
This unit test suite tests the rate of CAN messaging
on the bus. Measurement is performed while Tinymovr
is in position control mode.
'''
import time
import can

import tinymovr
from tinymovr import Tinymovr, ErrorIDs
from tinymovr.iface import IFace
from tinymovr.iface.can import CAN, guess_channel

import unittest

bustype = "slcan"
iterations = 5000

class TestRates(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        channel = guess_channel(bustype_hint=bustype)
        can_bus: can.Bus = can.Bus(bustype=bustype, channel=channel)
        iface: IFace = CAN(can_bus)
        cls.tm = Tinymovr(node_id=1, iface=iface)
        cls.tm.reset()
        time.sleep(0.2)

    def test_round_trip_time(self):
        '''
        Test round-trip message time (2 packets)
        '''
        self.try_calibrate()
        self.tm.position_control()
        time.sleep(0.2)
        elapsed_time()
        sum = 0
        for _ in range(iterations):
            sum += self.tm.encoder_estimates.position
        res = elapsed_time()
        print("Round-trip time (2 packets): " + str(res/iterations) + " seconds")

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


def elapsed_time(prefix=''):
    e_time = time.time()
    if not hasattr(elapsed_time, 's_time'):
        elapsed_time.s_time = e_time
    else:
        res = e_time - elapsed_time.s_time
        elapsed_time.s_time = e_time
        return res