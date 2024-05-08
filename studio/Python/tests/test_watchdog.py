"""
This unit test suite tests the Watchdog functionality.
"""
import time
import math

from avlos.unit_field import get_registry

import unittest
import pytest
from tests import TMTestCase

ureg = get_registry()
A = ureg.ampere
ticks = ureg.ticks
s = ureg.second
tsleep = 0.18

class TestWatchdog(TMTestCase):

    @pytest.mark.hitl_default
    def test_watchdog(self):
        """
        Test Watchdog function
        """
        self.check_state(0)
        self.try_calibrate()
        
        self.tm.watchdog.enabled = True
        self.assertEqual(self.tm.watchdog.enabled, True)
        self.tm.watchdog.timeout = 1.0 * s
        self.assertEqual(self.tm.watchdog.timeout, 1.0 * s)
        self.tm.controller.position_mode()
        self.check_state(2)
        for i in range(500):
            self.tm.controller.position.setpoint = math.sin(i * 0.05) * 16384
            self.assertEqual(self.tm.watchdog.triggered, False)
            time.sleep(0.01)
        self.check_state(2)
        time.sleep(2.0)
        # Watchdog should be triggered
        #self.assertEqual(self.tm.watchdog.triggered, True)
        # Watchdog should have switched controller to idle
        self.check_state(0)


if __name__ == "__main__":
    unittest.main(failfast=True)
