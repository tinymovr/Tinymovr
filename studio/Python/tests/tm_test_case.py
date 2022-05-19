"""
This unit test suite tests functionality
of Tinymovr boards.
"""
import random
import time
import can

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
        cls.reset_and_wait()

    def tearDown(self):
        self.tm.idle()

    @classmethod
    def tearDownClass(cls):
        cls.tm.reset()
        cls.can_bus.shutdown()

    @classmethod
    def reset_and_wait(cls, timeout=3):
        cls.tm.reset()
        time.sleep(timeout)

    def try_calibrate(self, force=False, *args, **kwargs):
        if True == force or self.tm.motor.calibrated:
            self.tm.calibrate()
            self.wait_for_calibration(*args, **kwargs)
            self.assertTrue(self.tm.motor.calibrated)

    def wait_for_calibration(self, check_interval=0.05):
        for _ in range(1000):
            if self.tm.controller.state == 0:
                break
            time.sleep(check_interval)
        self.assertEqual(self.tm.controller.state, 0)

    def check_state(self, target_state, target_error=None):
        errors = self.tm.errors
        if target_error and errors:
            self.assertGreater(errors & target_error.bitmask, 0)
        else:
            self.assertEqual(errors, 0)
        self.assertEqual(self.tm.controller.state, target_state)
