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


# def _recv_frame(self, timeout=0.1, sleep_interval=0.01):
#     # TODO: Move logic to Tee
#     total_interval = 0
#     while total_interval < timeout:
#         frame = self.bus.recv(0)
#         if frame:
#             return frame
#         time.sleep(sleep_interval)
#         total_interval += sleep_interval
#     raise ResponseError(self.node_id)

import time
from threading import Thread, Lock


tee = None


class Client:
    def __init__(self, filter_cb, recv_cb):
        self.filter_cb = filter_cb
        self.recv_cb = recv_cb


class Tee:
    """
    Distribute incoming messages based on the boolean result
    of a filter callback.

    python-can does not allow filtering messages per recipient therefore
    this class.

    Also implements a simple forwarding mechanism for sending messages, to
    simplify interfacing with CAN bus objects.
    """

    def __init__(self, bus, sleep_interval=0.05):
        self.bus = bus
        self.sleep_interval = sleep_interval
        self.lock = Lock()
        self.clients = []
        self.update_thread = Thread(target=self.update, daemon=True)
        self.update_thread.start()

    def add(self, filter_cb, recv_cb):
        self.clients.append(Client(filter_cb, recv_cb))

    def update(self):
        """

        """
        while True:  # TODO: while bus is active
            self.update_once()
            time.sleep(self.sleep_interval)

    def update_once(self):
        """
        Tries to receive a message from the bus object and if successful,
        tests reception of each tee instance in the global index.
        """
        self.lock.acquire()
        frame = self.bus.recv(0)
        if frame:
            for client in self.clients:
                if client.filter_cb(frame):
                    client.recv_cb(frame)
        self.lock.release()

    def send(self, frame):
        """
        Send a frame by forwarding to the bus object
        """
        self.lock.acquire()
        self.bus.send(frame)
        self.lock.release()


def init_tee(bus):
    global tee
    assert(None == tee)
    tee = Tee(bus)

def get_tee():
    return tee
