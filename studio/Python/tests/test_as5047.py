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
tsleep = 0.50


class TestAS5047(TMTestCase):

    @pytest.mark.sensor_as5047
    def test_a_position_control_before_after_save_and_load_config(self):
        """
        Test position control after saving and loading config.
        WARNING: This will perform one NVRAM write and two erase cycles.
        """
        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)

        self.configure_sensors(self.tm.sensors.setup.external_spi.type.AS5047)
        self.select_sensors(self.tm.sensors.select.position_sensor.connection.ONBOARD, self.tm.sensors.select.position_sensor.connection.EXTERNAL_SPI)

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

    @pytest.mark.sensor_as5047
    def test_b_position_control_following_sensor_change(self):
        """
        Test position control before and after changing sensor connection and type.
        This test will alter the sensor configuration from an external SPI connection
        to an onboard connection, and verify if the position control maintains accuracy.
        """
        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)

        # Initially configure with external SPI sensor
        self.configure_sensors(self.tm.sensors.setup.external_spi.type.AS5047)
        self.select_sensors(self.tm.sensors.select.position_sensor.connection.ONBOARD, self.tm.sensors.select.position_sensor.connection.EXTERNAL_SPI)
        self.try_calibrate()

        # Set initial controller gains
        self.tm.controller.position.p_gain = 9
        self.tm.controller.velocity.p_gain = 3e-5
        self.tm.controller.velocity.i_gain = 0
        self.tm.controller.velocity.limit = 200000

        # Start with external SPI sensor
        self.tm.controller.position_mode()
        self.check_state(2)

        for _ in range(5):
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

        # Change to onboard sensor
        self.select_sensors(self.tm.sensors.select.position_sensor.connection.ONBOARD, self.tm.sensors.select.position_sensor.connection.ONBOARD)

        # Re-calibrate with new sensor setup
        self.try_calibrate()
        time.sleep(0.1)

        self.tm.controller.position_mode()
        self.check_state(2)

        for _ in range(5):
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

    @pytest.mark.sensor_as5047
    def test_c_position_control_change_baud_rates(self):
        """
        Test position control with external sensor and
        different baud rates.
        """
        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)

        self.configure_sensors(self.tm.sensors.setup.external_spi.type.AS5047)
        self.select_sensors(self.tm.sensors.select.position_sensor.connection.ONBOARD, self.tm.sensors.select.position_sensor.connection.EXTERNAL_SPI)

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

        self.tm.sensors.setup.external_spi.rate = "1_5Mbps"
        time.sleep(0.1)

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


if __name__ == "__main__":
    unittest.main()
