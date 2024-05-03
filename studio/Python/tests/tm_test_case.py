"""
Tinymovr Base Test Class
Copyright Ioannis Chatzikonstantinou 2020-2023

Implements convenience functionality.

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

import os
import time
import can

from tinymovr import init_tee, destroy_tee
from tinymovr.config import get_bus_config, create_device

import unittest


class TMTestCase(unittest.TestCase):

    @classmethod
    def configure_sensors(cls, external_spi_type):
        cls.tm.sensors.setup.external_spi.type = external_spi_type
        time.sleep(0.2)

    @classmethod
    def select_sensors(cls, commutation_sensor_connection, position_sensor_connection):
        cls.tm.sensors.select.commutation_sensor.connection = commutation_sensor_connection
        cls.tm.sensors.select.position_sensor.connection = position_sensor_connection
        time.sleep(0.2)

    @classmethod
    def setUpClass(cls):
        params = get_bus_config(["canine", "slcan_disco"])
        params["bitrate"] = 1000000
        cls.can_bus = can.Bus(**params)
        init_tee(cls.can_bus)
        cls.tm = create_device(node_id=1)
        cls.reset_and_wait()

    def tearDown(self):
        self.tm.controller.idle()

    @classmethod
    def tearDownClass(cls):
        cls.tm.reset()
        destroy_tee()
        cls.can_bus.shutdown()

    @classmethod
    def reset_and_wait(cls, timeout=0.5):
        cls.tm.reset()
        time.sleep(timeout)

    def try_calibrate(self, I_cal=None, force=False, precheck_callback=None,*args, **kwargs):
        if True == force or not self.tm.calibrated:
            hw_rev = self.tm.hw_revision
            self.check_state(0)
            if I_cal and I_cal > 0:
                self.tm.motor.I_cal = I_cal
            elif hw_rev > 20:
                self.tm.motor.I_cal = 0.8
            else:
                self.tm.motor.I_cal = 5

            if hw_rev > 20:
                self.tm.controller.velocity.P_gain = 2e-05

            self.tm.controller.calibrate()
            self.wait_for_calibration(*args, **kwargs)
            if precheck_callback:
                precheck_callback(self.tm)
            self.assertTrue(self.tm.calibrated)

    def wait_for_calibration(self, check_interval=0.05):
        for _ in range(1000):
            if self.tm.controller.state == 0:
                break
            time.sleep(check_interval)
        self.assertEqual(self.tm.controller.state, 0)

    def check_state(self, target_state, target_error=None):
        errors = self.tm.errors
        if target_error and errors:
            self.assertGreater(errors & target_error.bitmask, 0)
        else:
            self.assertEqual(errors, 0)
        self.assertEqual(self.tm.controller.state, target_state)


# TODO: This is temporary, should be removed when
# slcan autodiscovery is merged in python-can
import serial
from serial.tools import list_ports
import logging


def guess_slcan():
    device_strings = ("canable", "cantact")
    ports = []
    for p in serial.tools.list_ports.comports():
        desc_lower: str = p.description.lower()
        if any([s in desc_lower for s in device_strings]):
            ports.append(p.device)
    if not ports:
        raise IOError("Could not autodiscover CAN channel")
    if len(ports) > 1:
        logging.warning("Multiple channels discovered - using the first")

    return ports[0]
