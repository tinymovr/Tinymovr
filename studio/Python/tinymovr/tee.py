"""
Tinymovr Tee Module
Copyright Ioannis Chatzikonstantinou 2020-2023

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


import time
from enum import Enum
from threading import Thread, Lock


tee = None


class Client:
    def __init__(self, filter_cb, recv_cb):
        self.filter_cb = filter_cb
        self.recv_cb = recv_cb


class TeeState(Enum):
    INIT = 0
    RUNNING = 1
    STOPPING = 2
    STOPPED = 3


class Tee:
    """
    Distribute incoming messages based on the boolean result
    of a filter callback.

    python-can does not allow filtering messages per recipient therefore
    this class.

    Also implements a simple forwarding mechanism for sending messages, to
    simplify interfacing with CAN bus objects.
    """

    def __init__(self, bus, timeout):
        self.bus = bus
        self.timeout = timeout
        self.clients = []
        self.state = TeeState.INIT
        self.update_thread = Thread(target=self.update, daemon=True)
        self.update_thread.start()

    def add(self, filter_cb, recv_cb):
        self.clients.append(Client(filter_cb, recv_cb))

    def update(self):
        """ """
        self.state = TeeState.RUNNING
        while TeeState.RUNNING == self.state:
            self._update_once()
        assert TeeState.STOPPING == self.state
        self.state = TeeState.STOPPED

    def _update_once(self):
        """
        Tries to receive a message from the bus object and if successful,
        tests reception of each tee instance in the global index.
        """
        frame = self.bus.recv(timeout=self.timeout)
        if frame:
            for client in self.clients:
                if client.filter_cb(frame):
                    client.recv_cb(frame)

    def send(self, frame):
        """
        Send a frame by forwarding to the bus object
        """
        self.bus.send(frame)

    def stop(self):
        self.state = TeeState.STOPPING
        while TeeState.STOPPING == self.state:
            time.sleep(0.01)
        assert TeeState.STOPPED == self.state


def init_tee(bus, timeout=0.1):
    """
    Initializes a tee using a python-can bus instance
    """
    global tee
    assert None == tee
    tee = Tee(bus, timeout)


def destroy_tee():
    """
    Destroys the existing tee, stopping its thread.
    """
    global tee
    assert None != tee
    tee.stop()
    tee = None


def get_tee():
    """
    Get the current tee object
    """
    return tee
