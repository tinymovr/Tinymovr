'''
This unit test suite tests functionality of the
Tinymovr Studio using a simulated Tinymovr
device, which is suitable for unit testing.
'''
import random
import time
import can

import tinymovr
from tinymovr import Tinymovr, ErrorIDs
from tinymovr.iface import IFace
from tinymovr.iface.can import CAN, guess_channel
from tinymovr.units import get_registry

import unittest

ureg = get_registry()
A = ureg.ampere
ticks = ureg.ticks
s = ureg.second

bustype = "slcan"

class TestBoard(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        channel = guess_channel(bustype_hint=bustype)
        can_bus: can.Bus = can.Bus(bustype=bustype, channel=channel)
        iface: IFace = CAN(can_bus)
        cls.tm = Tinymovr(node_id=1, iface=iface)
        cls.tm.reset()
        time.sleep(0.2)

    def test_e_save_load_config(self):
        '''
        Test erasing, saving and loading of config.
        WARNING: This will perform one NVRAM write and two erase cycles.
        '''
        state = self.tm.state
        self.assertEqual(state.error, ErrorIDs.NoError)
        self.assertEqual(state.state, 0)
        self.tm.erase_config()
        time.sleep(0.2)
        self.assertEqual(self.tm.motor_info.calibrated, 0)
        self.tm.calibrate()
        for i in range(100):
            if self.tm.state.state == 0:
                break
            time.sleep(0.5)
        self.assertEqual(self.tm.motor_info.calibrated, 1)
        self.tm.save_config()
        time.sleep(0.2)
        self.tm.reset()
        time.sleep(0.2)
        self.assertEqual(self.tm.motor_info.calibrated, 1)
        self.tm.erase_config()
        time.sleep(0.2)

    def tearDown(self):
        self.tm.idle()

    @classmethod
    def tearDownClass(cls):
        cls.tm.reset()


if __name__ == '__main__':
    unittest.main()