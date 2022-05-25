"""
This unit test suite tests functionality
of Tinymovr boards.
"""
import yaml
import time
import can

from tinymovr.config import get_bus_config, create_device

import unittest


class TMTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        bustype, channel = get_bus_config(["socketcan"])
        cls.can_bus = can.Bus(
            bustype=bustype, channel=channel, bitrate=1000000
        )
        cls.tm = create_device(node_id=1, bus=cls.can_bus)
        cls.reset_and_wait()

    def tearDown(self):
        self.tm.controller.idle_mode()

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
