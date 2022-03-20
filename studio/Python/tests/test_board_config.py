'''
This unit test suite tests functionality of the
Tinymovr Studio using a simulated Tinymovr
device, which is suitable for unit testing.
'''
import random
import time

import tinymovr
from tinymovr import Tinymovr
from tinymovr.constants import ErrorIDs
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

        self.reset_and_wait()

        self.check_state(0)
        self.assertEqual(self.tm.motor_config.flags, 0)

        self.tm.position_control()
        self.check_state(0, ErrorIDs.InvalidState)

        self.reset_and_wait()
        
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

        self.reset_and_wait()


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
        R = self.tm.motor_RL.R
        L = self.tm.motor_RL.L
        pole_pairs = self.tm.motor_config.pole_pairs
        self.tm.save_config()
        time.sleep(0.2)
        self.reset_and_wait()
        self.assertEqual(self.tm.motor_config.flags, 1)
        self.assertAlmostEqual(R, self.tm.motor_RL.R)
        self.assertAlmostEqual(L, self.tm.motor_RL.L)
        self.assertAlmostEqual(pole_pairs, self.tm.motor_config.pole_pairs)
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
        self.tm.set_vel_integrator_params(2e-2)
        self.tm.set_limits(120000, 18)
        self.tm.save_config()
        time.sleep(0.2)
        self.reset_and_wait()
        self.assertAlmostEqual(self.tm.gains.position, 30 * 1/s)
        self.assertAlmostEqual(self.tm.gains.velocity, 3e-5 * A*s/tick)
        self.assertAlmostEqual(self.tm.integrator_gains, 2e-2 * A*s/tick)
        self.assertAlmostEqual(self.tm.limits.velocity, 120000 * tick/s)
        self.assertAlmostEqual(self.tm.limits.current, 18 * A)
        self.tm.erase_config()
        time.sleep(0.2)


if __name__ == '__main__':
    unittest.main()