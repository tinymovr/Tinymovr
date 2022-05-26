"""
Tinymovr Tee Module
Copyright Ioannis Chatzikonstantinou 2020-2022

Implements a Tee class to distribute incoming traffic according to rules

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

from threading import Lock


class Tee:
    """
    Sistribute incoming messages based on the boolean result
    of a filter callback.

    python-can does not allow filtering messages per recipient therefore
    this class.

    Also implements a simple forwarding mechanism for sending messages, to
    simplify interfacing with CAN bus objects.
    """

    tees = []
    lock = Lock()

    def __init__(self, bus, filter_cb):
        self.bus = bus
        self.filter_cb = filter_cb
        self.queue = []
        self.tees.append(self)

    def recv(self, *args, **kwargs):
        """
        Tries to receive a message from the bus object and if successful,
        tests reception of each tee instance in the global index.
        """
        self.lock.acquire()
        response = None
        frame = self.bus.recv(*args, **kwargs)
        if frame:
            for tee in self.tees:
                if tee.filter_cb(frame):
                    tee.queue.append(frame)
        try:
            response = self.queue.pop(0)
        except IndexError:
            pass
        self.lock.release()
        return response

    def send(self, frame):
        """
        Send a frame by forwarding to the bus object
        """
        self.lock.acquire()
        self.bus.send(frame)
        self.lock.release()
