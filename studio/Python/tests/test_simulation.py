"""
Tinymovr Simulation Tests
Copyright Ioannis Chatzikonstantinou 2020-2023

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

import unittest
from unittest.mock import patch, MagicMock
from tinymovr import init_router, destroy_router
from tinymovr.channel import ResponseError
from tinymovr.config import create_device

class TestSimulation(unittest.TestCase):
    
    @patch("can.Bus")
    def test_response_error(self, mock_can_bus_class):
        """
        Test that an appropriate error is raised when the device receives an erroneous response.
        """
        # Setup mock
        mock_can_bus_instance = MagicMock()
        mock_can_bus_class.return_value = mock_can_bus_instance
        mock_params = MagicMock()
        mock_logger = MagicMock()
        
        # Initialize the router with the mocked class and parameters
        init_router(mock_can_bus_class, mock_params, logger=mock_logger)
        
        # Test that an error is raised during device creation
        with self.assertRaises(ResponseError):
            create_device(node_id=1)
        
        # Check if send and recv were called on the can bus instance
        assert mock_can_bus_instance.send.called
        assert mock_can_bus_instance.recv.called
        
        # Clean up router
        destroy_router()

if __name__ == "__main__":
    unittest.main()
