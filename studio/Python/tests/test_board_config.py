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
tick = ureg.ticks
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

        self.try_calibrate()

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
        self.try_calibrate()
        self.assertEqual(self.tm.motor_config.flags, 1)
        self.tm.save_config()
        time.sleep(0.2)
        self.tm.reset()
        time.sleep(0.2)
        self.assertEqual(self.tm.motor_config.flags, 1)
        self.tm.erase_config()
        time.sleep(0.2)

    def test_c_parameter_persistence(self):
        '''
        Test persisting parameters across config saves.
        WARNING: This will perform one NVRAM write and two erase cycles.
        '''
        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)
        self.tm.set_gains(30, 3e-5)
        self.tm.set_integrator_gains(2e-2)
        self.tm.set_limits(120000, 18)
        self.tm.save_config()
        time.sleep(0.2)
        self.tm.reset()
        time.sleep(0.2)
        self.assertAlmostEqual(self.tm.gains.position, 30 * 1/s)
        self.assertAlmostEqual(self.tm.gains.velocity, 3e-5 * A*s/tick)
        self.assertAlmostEqual(self.tm.integrator_gains, 2e-2 * A*s/tick)
        self.assertAlmostEqual(self.tm.limits.velocity, 120000 * tick/s)
        self.assertAlmostEqual(self.tm.limits.current, 18 * A)
        self.tm.erase_config()
        time.sleep(0.2)


if __name__ == '__main__':
    unittest.main()