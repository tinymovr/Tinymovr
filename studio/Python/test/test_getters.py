import random
import time
import can

import tinymovr
from tinymovr import Tinymovr, ErrorIDs
from tinymovr.iface import IFace
from tinymovr.iface.can import CAN

import unittest

class TestGetters(unittest.TestCase):

    def setUp(self):
        bustype = "tinymovr_test"
        channel = "test_channel"
        can_bus: can.Bus = can.Bus(bustype=bustype,
                               channel=channel)
        iface: IFace = CAN(can_bus)
        self.tm: Tinymovr = Tinymovr(node_id=1, iface=iface)

    def test_get_device_info(self):
        info = self.tm.device_info
        self.assertGreaterEqual(info.fw_major, 0)
        self.assertGreaterEqual(info.fw_minor, 7)

    def test_get_error(self):
        state = self.tm.state
        error_id = ErrorIDs(state.error)
        self.assertEqual(error_id, ErrorIDs.NoError)

    def test_get_encoder_estimates(self):
        estimates = self.tm.encoder_estimates
        self.assertEqual(estimates.position, 0)
        self.assertEqual(estimates.velocity, 0)



if __name__ == '__main__':
    unittest.main()