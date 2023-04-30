"""
Tinymovr Trajectory Planner Tests
Copyright Ioannis Chatzikonstantinou 2020-2023

Tests the functionality of the Tinymovr trajectory planner.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
"""

import random
import time
import statistics as st

import unittest
from tests import TMTestCase


class TestTrajPlanner(TMTestCase):
    def test_traj_planner(self):
        """
        Test trajectory planner
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        time.sleep(1)

        self.tm.traj_planner.max_vel = 100000

        for _ in range(20):
            self.tm.traj_planner.move_to(random.randrange(-200000, 200000))
            time.sleep(0.6)

        self.tm.traj_planner.move_to(self.tm.controller.position.setpoint)
        time.sleep(4)

    def test_traj_planner_alt(self):
        """
        Test trajectory planner with alternate acceleration and velocity limits
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        time.sleep(1)

        self.tm.traj_planner.max_vel = 100000
        self.tm.traj_planner.max_accel = 50000
        self.tm.traj_planner.max_decel = 50000

        for _ in range(20):
            self.tm.traj_planner.move_to(random.randrange(-200000, 200000))
            time.sleep(0.6)

        self.tm.traj_planner.move_to(self.tm.controller.position.setpoint)
        time.sleep(4)

    def test_traj_planner_tlimit(self):
        """
        Test trajectory planner with time limits
        """
        self.check_state(0)
        self.try_calibrate()
        self.tm.controller.position_mode()
        self.check_state(2)

        time.sleep(1)

        self.tm.traj_planner.max_vel = 200000
        self.tm.traj_planner.t_accel = 1
        self.tm.traj_planner.t_decel = 1
        self.tm.traj_planner.t_total = 3

        for _ in range(5):
            self.tm.traj_planner.move_to_tlimit(random.randrange(-100000, 100000))
            time.sleep(0.6)

        self.tm.traj_planner.move_to_tlimit(self.tm.controller.position.setpoint)
        time.sleep(4)

if __name__ == "__main__":
    unittest.main(failfast=True)
