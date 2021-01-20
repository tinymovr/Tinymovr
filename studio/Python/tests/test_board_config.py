'''
This unit test suite tests functionality of the
Tinymovr Studio using a simulated Tinymovr
device, which is suitable for unit testing.
'''
import random
import time
import can

import tinymovr
from tinymovr import Tinymovr
from tinymovr.constants import ErrorIDs
from tinymovr.iface import IFace
from tinymovr.iface.can import CAN, guess_channel
from tinymovr.units import get_registry

import unittest
from tests import TMTestCase

ureg = get_registry()
A = ureg.ampere
ticks = ureg.ticks
s = ureg.second


class TestBoardConfig(TMTestCase):
    
    def test_a_state_errors(self):
        '''
        Test state transitions
        WARNING: This will perform one NVRAM erase cycle.
        '''
        if self.tm.motor_config.flags == 1:
            self.tm.erase_config()
            time.sleep(0.2)

        self.tm.reset()
        time.sleep(0.2)

        self.check_state(0)
        self.assertEqual(self.tm.motor_config.flags, 0)

        self.tm.position_control()
        self.check_state(0, ErrorIDs.InvalidState)

        self.tm.reset()
        time.sleep(0.2)
        
        self.check_state(0)
        self.assertEqual(self.tm.motor_config.flags, 0)

        self.tm.calibrate()
        for _ in range(100):
            if self.tm.state.state == 0:
                break
            time.sleep(0.5)

        self.assertEqual(self.tm.motor_config.flags, 1)
        self.check_state(0)
        time.sleep(0.2)

        self.tm.position_control()
        self.check_state(2)
        time.sleep(0.2)

        self.tm.calibrate()
        self.check_state(0, ErrorIDs.InvalidState)

        self.tm.reset()
        time.sleep(0.2)


    def test_b_save_load_config(self):
        '''
        Test erasing, saving and loading of config.
        WARNING: This will perform one NVRAM write and two erase cycles.
        '''
        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)
        self.assertEqual(self.tm.motor_config.flags, 0)
        self.tm.calibrate()
        for _ in range(100):
            if self.tm.state.state == 0:
                break
            time.sleep(0.5)
        self.assertEqual(self.tm.motor_config.flags, 1)
        self.tm.save_config()
        time.sleep(0.2)
        self.tm.reset()
        time.sleep(0.2)
        self.assertEqual(self.tm.motor_config.flags, 1)
        self.tm.erase_config()
        time.sleep(0.2)


if __name__ == '__main__':
    unittest.main()