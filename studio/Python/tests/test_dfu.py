"""
Tinymovr DFU Test Class
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

import time
import can

try:
    from rd6006 import RD6006
    rd = RD6006('/dev/tty.usbserial-2110')
    psu_connected = True
except:
    psu_connected = False

from tinymovr import init_router, destroy_router
from tinymovr.config import (
    get_bus_config,
    create_device,
    configure_logging
)

import unittest
import pytest

class TMTestCase(unittest.TestCase):

    @classmethod
    def setUp(cls):
        cls.logger = configure_logging()
        cls.params = get_bus_config(["canine", "slcan_disco"], bitrate=1000000)
        init_router(can.Bus, cls.params, logger=cls.logger)

    @pytest.mark.dfu
    def test_dfu(self, node_id=1):
        time.sleep(0.5)
        for i in range(10):
            print("Testing DFU iteration ", i+1)
            tm = create_device(node_id=node_id)
            tm_hash = tm.protocol_hash
            tm.enter_dfu()
            time.sleep(0.5)
            bl = create_device(node_id=node_id)
            bl_hash = bl.protocol_hash
            bl.reset()
            time.sleep(0.1)
            tm = create_device(node_id=node_id)
            tm_hash2 = tm.protocol_hash
            tm.reset()
            time.sleep(0.1)
            tm_hash3 = tm.protocol_hash
            self.assertNotEqual(tm_hash, 0)
            self.assertEqual(tm_hash, tm_hash2)
            self.assertEqual(tm_hash, tm_hash3)
            self.assertNotEqual(tm_hash, bl_hash)
            time.sleep(0.1)

    @pytest.mark.dfu
    def test_dfu_power_off(self, node_id=1):
        if psu_connected == False:
            raise unittest.SkipTest("Skipping test because no connected RD series PSU found")
        time.sleep(0.5)
        for i in range(3):
            print("Testing DFU iteration ", i+1)
            tm = create_device(node_id=node_id)
            tm_hash = tm.protocol_hash
            tm.enter_dfu()
            time.sleep(0.5)
            bl = create_device(node_id=node_id)
            bl_hash = bl.protocol_hash
            rd.enable=False
            time.sleep(2.0)
            rd.enable=True
            time.sleep(0.5)
            tm = create_device(node_id=node_id)
            tm_hash2 = tm.protocol_hash
            rd.enable=False
            time.sleep(2.0)
            rd.enable=True
            time.sleep(0.5)
            tm_hash3 = tm.protocol_hash
            self.assertNotEqual(tm_hash, 0)
            self.assertEqual(tm_hash, tm_hash2)
            self.assertEqual(tm_hash, tm_hash3)
            self.assertNotEqual(tm_hash, bl_hash)
            time.sleep(0.1)

    @classmethod
    def tearDownClass(cls):
        destroy_router()


if __name__ == "__main__":
    unittest.main(failfast=True)
