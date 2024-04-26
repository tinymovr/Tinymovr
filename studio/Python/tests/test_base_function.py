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
        hw_rev = self.tm.hw_revision
        self.check_state(0)
        if hw_rev > 20:
            self.tm.motor.I_cal = 0.8
        else:
            self.tm.motor.I_cal = 5
        self.tm.controller.current.Iq_limit = 5
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        for i in range(5):
            self.tm.controller.position.setpoint = i * 3000 * ticks
            if hw_rev > 20:
                time.sleep(0.50)
            else:
                time.sleep(0.25)
            self.assertAlmostEqual(
                i * 3000 * ticks, self.tm.sensors.user_frame.position_estimate, delta=1000 * ticks
            )



if __name__ == "__main__":
    unittest.main(failfast=True)
