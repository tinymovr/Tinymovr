"""
This unit test suite tests functionality of the
Tinymovr Studio using a simulated Tinymovr
device, which is suitable for unit testing.
"""

import logging
import can

from avlos import get_object_tree

from tinymovr.isotp_channel import ISOTPChannel
from tinymovr.constants import ErrorIDs

import unittest

NODE_ID = 1

class TestSimulation(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        can_bus = can.Bus(bustype="sim", channel="sim", node_id=1)
        cls.logger = logging.getLogger("tinymovr")
        cls.logger.setLevel(logging.DEBUG)
        isotp_chan = ISOTPChannel(can_bus, NODE_ID, cls.logger)
        cls.tm = get_object_tree(isotp_chan)

    def setUp(self):
        self.tm.reset()

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
        self.assertAlmostEqual(0.02, self.tm.motor.R)
        self.assertAlmostEqual(0.0005, self.tm.motor.L)



if __name__ == "__main__":
    unittest.main()