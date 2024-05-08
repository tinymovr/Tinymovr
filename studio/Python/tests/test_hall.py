"""
Tinymovr Board Config Tests
Copyright Ioannis Chatzikonstantinou 2020-2023

Tests saving and loading Tinymovr configuration to/from NVRAM.

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

from avlos.unit_field import get_registry

import unittest
import pytest
from tests import TMTestCase

ureg = get_registry()
A = ureg.ampere
tick = ureg.ticks
s = ureg.second
tsleep = 1.00


def set_pole_pairs(tm):
    tm.motor.pole_pairs = 15

class TestHall(TMTestCase):

    @pytest.mark.sensor_hall
    def test_a_position_control_w_loaded_config(self):
        """
        Test position control after saving and loading config.
        WARNING: This will perform one NVRAM write and two erase cycles.
        """
        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)

        self.tm.sensors.select.position_sensor.connection = self.tm.sensors.select.position_sensor.connection.HALL
        self.tm.sensors.select.commutation_sensor.connection = self.tm.sensors.select.commutation_sensor.connection.HALL
        # Commutation sensor remains internal

        self.tm.sensors.select.position_sensor.bandwidth = 50;
        self.tm.sensors.select.commutation_sensor.bandwidth = 80;
        time.sleep(0.2)

        self.try_calibrate(precheck_callback=set_pole_pairs)

        self.tm.controller.position.p_gain = 15
        self.tm.controller.velocity.limit = 400000
        self.tm.controller.velocity.p_gain = 1e-5
        self.tm.controller.velocity.i_gain = 0

        self.tm.controller.position_mode()
        self.check_state(2)

        for _ in range(5):
            new_pos = random.uniform(-200000, 200000)
            self.tm.controller.position.setpoint = new_pos * tick
            time.sleep(tsleep)
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.position_estimate,
                self.tm.controller.position.setpoint,
                delta=30000 * tick,
            )

        self.tm.controller.idle()

        time.sleep(0.1)

        self.tm.save_config()
        
        time.sleep(0.2)

        self.reset_and_wait()
        
        self.tm.controller.position_mode()
        self.check_state(2)

        for _ in range(10):
            new_pos = random.uniform(-200000, 200000)
            self.tm.controller.position.setpoint = new_pos * tick
            time.sleep(tsleep)
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.position_estimate,
                self.tm.controller.position.setpoint,
                delta=30000 * tick,
            )

        self.tm.erase_config()
        time.sleep(0.2)


if __name__ == "__main__":
    unittest.main()
