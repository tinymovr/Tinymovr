'''
This unit test suite tests functionality
of Tinymovr boards.
'''
import random
import time
import can
import statistics as st

import tinymovr
from tinymovr import Tinymovr
from tinymovr.iface import IFace
from tinymovr.iface.can import CAN, guess_channel
from tinymovr.units import get_registry

import unittest

bustype = "slcan"


class TMTestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        channel = guess_channel(bustype_hint=bustype)
        cls.can_bus: can.Bus = can.Bus(bustype=bustype, channel=channel, bitrate=1000000)
        iface: IFace = CAN(cls.can_bus)
        cls.tm = Tinymovr(node_id=1, iface=iface)
        cls.tm.reset()
        time.sleep(0.2)

    def tearDown(self):
        self.tm.idle()

    @classmethod
    def tearDownClass(cls):
        cls.tm.reset()
        cls.can_bus.shutdown()

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

    def check_state(self, target_state, target_error=None):
        state = self.tm.state
        if target_error:
            self.assertIn(target_error, state.errors)
        else:
            self.assertFalse(state.errors)
        self.assertEqual(state.state, target_state)

