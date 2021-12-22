"""
This unit test suite tests functionality of the
Tinymovr Studio using a simulated Tinymovr
device, which is suitable for unit testing.
"""

import logging
import can

from tinymovr.isotp_channel import ISOTPChannel
from tinymovr.constants import ErrorIDs

import unittest

def get_tm(logger):
    can_bus = can.Bus(bustype="sim", channel="test")
    return ISOTPChannel(can_bus, 1, logger)


class TestSimulation(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.logger = logging.getLogger("tinymovr")
        cls.logger.setLevel(logging.DEBUG)
        cls.tm = get_tm()

    def setUp(self):
        self.tm.reset()

    def test_get_device_info(self):
        """
        Test getting of device info
        """
        pass



if __name__ == "__main__":
    unittest.main()