"""
This unit test suite tests the Homing functionality.
"""
import time
import math

from avlos.unit_field import get_registry

import unittest
from tests import TMTestCase

ureg = get_registry()
A = ureg.ampere
ticks = ureg.ticks
s = ureg.second
tsleep = 0.18


class TestHoming(TMTestCase):
    def test_homing(self):
        """
        Test Homing
        """
        self.check_state(0)
        self.try_calibrate()

        self.tm.controller.position_mode()
        self.check_state(2)

        self.tm.homing.home()
        time.sleep(self.tm.homing.max_homing_t.magnitude + 0.1)

        self.assertEqual(self.tm.controller.mode, 2)
        self.assertEqual(self.tm.homing.warnings, 1)
        self.tm.controller.idle()


if __name__ == "__main__":
    unittest.main(failfast=True)
