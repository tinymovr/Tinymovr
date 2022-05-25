'''
This unit test suite tests functionality
of Tinymovr trajectory planner.
'''
import random
import time
import statistics as st

import tinymovr
from tinymovr import Tinymovr

import unittest
from tests import TMTestCase


class TestTrajPlanner(TMTestCase):

    def test_traj_planner(self):
        '''
        Test trajectory planner
        '''
        self.check_state(0)
        self.try_calibrate()
        self.tm.position_control()
        self.check_state(2)

        time.sleep(1)

        self.tm.traj_planner.max_vel = 100000

        for _ in range(20):
            self.tm.traj_planner.move_to(random.randrange(-200000, 200000))
            time.sleep(0.6)
        
        self.tm.traj_planner.move_to(self.tm.controller.pos_setpoint)
        time.sleep(4)

    def test_traj_planner_alt(self):
        '''
        Test trajectory planner with alternate acceleration and velocity limits
        '''
        self.check_state(0)
        self.try_calibrate()
        self.tm.position_control()
        self.check_state(2)

        time.sleep(1)

        self.tm.traj_planner.max_vel = 100000
        self.tm.traj_planner.max_accel = 50000
        self.tm.traj_planner.max_decel = 50000

        for _ in range(20):
            self.tm.traj_planner.move_to(random.randrange(-200000, 200000))
            time.sleep(0.6)
        
        self.tm.traj_planner.move_to(self.tm.controller.pos_setpoint)
        time.sleep(4)

if __name__ == '__main__':
    unittest.main(failfast=True)
