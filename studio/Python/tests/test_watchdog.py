"""
This unit test suite tests the Watchdog functionality.
"""
import time
import math

from tinymovr.units import get_registry

import unittest
from tests import TMTestCase

ureg = get_registry()
ticks = ureg.ticks
s = ureg.second


class TestWatchdog(TMTestCase):
    def test_watchdog(self):
        """
        Test Watchdog function
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.position_control()
        self.check_state(2)
        self.tm.set_watchdog(1, 1)
        for i in range(500):
            self.tm.set_pos_setpoint(math.sin(i * 0.05) * 16384)
            time.sleep(0.02)
        self.check_state(2)
        time.sleep(1.5)
        # Watchdog should have switched controller to idle
        self.check_state(0)


if __name__ == "__main__":
    unittest.main(failfast=True)
