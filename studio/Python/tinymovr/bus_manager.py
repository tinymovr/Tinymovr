"""
Tinymovr BusManager Module
Copyright Ioannis Chatzikonstantinou 2020-2023

Implements a BusRouter class to distribute incoming traffic according to rules

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

from can.exceptions import CanOperationError, CanInitializationError

class BusManager:
    def __init__(self, bus_class, bus_params, logger, bus_exceptions=(CanOperationError, CanInitializationError)):
        self.bus = None
        self.bus_class = bus_class
        self.bus_params = bus_params
        self.bus_exceptions = bus_exceptions
        self.logger = logger
        self.attempt_reconnect()

    def attempt_reconnect(self):
        try:
            bus = self.bus_class(**self.bus_params)
            # Flush bus to discard buffered data
            # Also acts as check for bus exception
            self.flush_rx_buffer(bus)
            self.bus = bus
            self.logger.info("Bus connected")
        except self.bus_exceptions as e:
            self.bus = None

    def is_connected(self):
        return self.bus != None

    def recv(self, timeout):
        try:
            return self.bus.recv(timeout=timeout)
        except self.bus_exceptions as e:
            self.logger.warn(e)
            self.bus = None
            self.attempt_reconnect()
        except AttributeError:
            self.attempt_reconnect()
    
    def shutdown(self):
        try:
            self.bus.shutdown()
            self.bus = None
        except AttributeError:
            pass
        
    def send(self, frame):
        try:
            self.bus.send(frame)
        except self.bus_exceptions as e:
            self.logger.warn(e)
            self.attempt_reconnect()
        except AttributeError:
            self.attempt_reconnect()

    def flush_rx_buffer(self, bus=None, trials=100):
        """
        Flush the RX buffer of a bus
        """
        if bus == None:
            bus = self.bus
        for i in range(trials):
            if not bus.recv(timeout=0.001):
                return