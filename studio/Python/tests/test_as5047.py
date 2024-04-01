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
from tests import TMTestCase

ureg = get_registry()
A = ureg.ampere
tick = ureg.ticks
s = ureg.second
tsleep = 0.50


class TestAS5047(TMTestCase):

    @classmethod
    def configure_sensors(cls):
        cls.tm.sensors.setup.external_spi.type = cls.tm.sensors.setup.external_spi.type.AS5047
        cls.tm.sensors.select.position_sensor.connection = cls.tm.sensors.select.position_sensor.connection.EXTERNAL_SPI
        # Commutation sensor remains internal
        time.sleep(0.2)

    @classmethod
    def setUpClass(cls):
        super(TestAS5047, cls).setUpClass()
        
        cls.configure_sensors()
        
        # We look at the position estimate, which is highly unlikely to be exactly zero.
        if cls.tm.sensors.select.position_sensor.raw_angle == 16383:
            raise unittest.SkipTest("AS5047 sensor not present. Skipping test.")

        cls.reset_and_wait()

    def test_a_position_control_w_loaded_config(self):
        """
        Test position control after saving and loading config.
        WARNING: This will perform one NVRAM write and two erase cycles.
        """
        pass
        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)

        self.configure_sensors()

        self.tm.motor.I_cal = 0.8

        self.try_calibrate()

        self.tm.controller.position.p_gain = 9
        self.tm.controller.velocity.p_gain = 2e-5
        self.tm.controller.velocity.i_gain = 0

        self.tm.controller.position_mode()
        self.check_state(2)

        for _ in range(10):
            new_pos = random.uniform(-20000, 20000)
            self.tm.controller.position.setpoint = new_pos * tick
            time.sleep(tsleep)
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.position_estimate,
                self.tm.controller.position.setpoint,
                delta=2000 * tick,
            )

        self.tm.controller.idle()

        time.sleep(0.1)

        self.tm.save_config()
        
        time.sleep(0.2)

        self.reset_and_wait()
        
        self.tm.controller.position_mode()
        self.check_state(2)

        for _ in range(10):
            new_pos = random.uniform(-20000, 20000)
            self.tm.controller.position.setpoint = new_pos * tick
            time.sleep(tsleep)
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.position_estimate,
                self.tm.controller.position.setpoint,
                delta=2000 * tick,
            )

        self.tm.erase_config()
        time.sleep(0.2)


if __name__ == "__main__":
    unittest.main()
