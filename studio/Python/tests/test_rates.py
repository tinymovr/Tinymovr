"""
Tinymovr Message Rate Tests
Copyright Ioannis Chatzikonstantinou 2020-2023

Tests the rate of CAN messages on the bus.

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

import time
from tests import TMTestCase

import pytest

iterations = 5000


class TestRates(TMTestCase):

    @pytest.mark.hitl_default
    def test_round_trip_time(self):
        """
        Test round-trip message time (2 packets)
        """
        self.try_calibrate()
        self.tm.controller.position_mode()
        time.sleep(0.2)
        elapsed_time()
        sum = 0
        for _ in range(iterations):
            sum += self.tm.sensors.user_frame.position_estimate
        res = elapsed_time()
        print("Round-trip time (2 packets): " + str(res / iterations) + " seconds")

    # def test_round_trip_time_with_write(self):
    #     """
    #     Test round-trip message time of r/w endpoints (2 packets)
    #     """
    #     self.try_calibrate()
    #     self.tm.controller.position_mode()
    #     time.sleep(0.2)
    #     elapsed_time()
    #     sum = 0
    #     pos = self.tm.encoder.position_estimate
    #     for _ in range(iterations):
    #         sum += self.tm.get_set_pos_vel(pos, 0).position
    #     res = elapsed_time()
    #     print("Round-trip time (2 packets, rw): " + str(res / iterations) + " seconds")


def elapsed_time(prefix=""):
    e_time = time.time()
    if not hasattr(elapsed_time, "s_time"):
        elapsed_time.s_time = e_time
    else:
        res = e_time - elapsed_time.s_time
        elapsed_time.s_time = e_time
        return res
