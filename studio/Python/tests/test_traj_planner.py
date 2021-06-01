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

        for _ in range(20):
            self.tm.plan_v_limit(random.randrange(-200000, 200000), 100000)
            time.sleep(0.6)
        
        self.tm.plan_v_limit(self.tm.setpoints.position, 100000)
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

        self.tm.set_max_plan_acc_dec(50000, 50000)

        for _ in range(20):
            self.tm.plan_v_limit(random.randrange(-200000, 200000), 180000)
            time.sleep(0.6)
        
        self.tm.plan_v_limit(self.tm.setpoints.position, 100000)
        time.sleep(4)

if __name__ == '__main__':
    unittest.main(failfast=True)
