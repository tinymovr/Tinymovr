"""
Tinymovr Simulation Tests
Copyright Ioannis Chatzikonstantinou 2020-2022

Tests functionality of the Tinymovr Studio using a simulated Tinymovr device.

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

from tinymovr import init_tee
from tinymovr.channel import ResponseError
from tinymovr.config import create_device
from unittest.mock import patch, MagicMock
import unittest


class TestSimulation(unittest.TestCase):
    @patch("can.Bus")
    def test_response_error(self, can_bus):
        """
        Test response error
        """
        can_bus.send = MagicMock()
        can_bus.recv = MagicMock()
        init_tee(can_bus)
        with self.assertRaises(ResponseError):
            tm = create_device(node_id=1)
        assert can_bus.send.called
        assert can_bus.recv.called


if __name__ == "__main__":
    unittest.main()
