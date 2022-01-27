"""
This unit test suite tests functionality of the
Tinymovr Studio using a simulated Tinymovr
device, which is suitable for unit testing.
"""

from tests import TMTestCase
import unittest


class TestSimulation(TMTestCase):

    def test_set_get_pos(self):
        """
        Test setting and getting position
        """
        self.assertAlmostEqual(0, self.tm.controller.pos_setpoint)
        self.tm.controller.pos_setpoint = 5.0
        self.assertAlmostEqual(5, self.tm.controller.pos_setpoint)
        self.tm.controller.pos_setpoint = -100000.0
        self.assertAlmostEqual(-100000.0, self.tm.controller.pos_setpoint)

    def test_set_get_vel(self):
        """
        Test setting and getting velocity
        """
        self.assertAlmostEqual(0, self.tm.controller.vel_setpoint)
        self.tm.controller.vel_setpoint = 5.0
        self.assertAlmostEqual(5, self.tm.controller.vel_setpoint)
        self.tm.controller.vel_setpoint = -100000.0
        self.assertAlmostEqual(-100000.0, self.tm.controller.vel_setpoint)

    def test_get_RL(self):
        """
        Test getting resistance and inductance
        """
        self.assertAlmostEqual(0.2, self.tm.motor.R)
        self.assertAlmostEqual(0.0005, self.tm.motor.L)


if __name__ == "__main__":
    unittest.main()
