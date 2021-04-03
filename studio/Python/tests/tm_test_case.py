"""
This unit test suite tests functionality
of Tinymovr boards.
"""
import random
import time
import can
import statistics as st

import tinymovr
from tinymovr import Tinymovr
from tinymovr.iface import IFace
from tinymovr.iface.can_bus import CANBus, guess_channel
from tinymovr.units import get_registry

import unittest

bustype = "slcan"


class TMTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        channel = guess_channel(bustype_hint=bustype)
        cls.can_bus: can.Bus = can.Bus(
            bustype=bustype, channel=channel, bitrate=1000000
        )
        iface: IFace = CANBus(cls.can_bus)
        cls.tm = Tinymovr(node_id=1, iface=iface)
        cls.tm.reset()
        time.sleep(0.2)

    def tearDown(self):
        self.tm.idle()

    @classmethod
    def tearDownClass(cls):
        cls.tm.reset()
        cls.can_bus.shutdown()

    def try_calibrate(self, *args, **kwargs):
        motor_config = self.tm.motor_config
        if motor_config.flags == 0 or motor_config.flags == 2:
            self.tm.calibrate()
            self.wait_for_calibration(*args, **kwargs)
            motor_config = self.tm.motor_config
            self.assertTrue(motor_config.flags == 1 or motor_config.flags == 3)

    def wait_for_calibration(self, check_interval=0.05):
        for _ in range(1000):
            if self.tm.state.state == 0:
                break
            time.sleep(check_interval)
        self.assertEqual(self.tm.state.state, 0)

    def check_state(self, target_state, target_error=None):
        state = self.tm.state
        if target_error:
            self.assertIn(target_error, state.errors)
        else:
            self.assertFalse(state.errors)
        self.assertEqual(state.state, target_state)
