"""
This unit test suite tests functionality
of Tinymovr boards.
"""
import time

from avlos.unit_field import get_registry

import unittest
from tests import TMTestCase

ureg = get_registry()
A = ureg.ampere
ticks = ureg.ticks
s = ureg.second


class TestBaseFunction(TMTestCase):
    
    def test_position_control(self):
        """
        Test position control
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        for i in range(5):
            self.tm.controller.position.setpoint = i * 10000 * ticks
            time.sleep(0.25)
            self.assertAlmostEqual(
                i * 10000 * ticks, self.tm.encoder.position_estimate, delta=1000 * ticks
            )



if __name__ == "__main__":
    unittest.main(failfast=True)
