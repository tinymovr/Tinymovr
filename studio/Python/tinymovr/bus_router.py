"""
Tinymovr BusRouter Module
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


import warnings
from enum import Enum
from threading import Thread, Lock
from tinymovr.bus_manager import BusManager
from logging import Logger


bus_router = None


class Client:
    def __init__(self, filter_cb, recv_cb):
        self.filter_cb = filter_cb
        self.recv_cb = recv_cb


class BusRouterState(Enum):
    INIT = 0
    RUNNING = 1
    STOPPING = 2
    STOPPED = 3


class BusRouter:
    """
    Distribute incoming messages based on the boolean result
    of a filter callback.

    python-can does not allow filtering messages per recipient therefore
    this class.

    Also implements a simple forwarding mechanism for sending messages, to
    simplify interfacing with CAN bus objects.
    """

    def __init__(self, bus_manager, timeout, logger):
        self.bus_manager = bus_manager
        self.timeout = timeout
        self.logger = logger
        self.clients = []
        self.running = True
        self.update_thread = Thread(target=self.run, daemon=True)
        self.update_thread.start()

    def run(self):
        while self.running:
            frame = self.bus_manager.recv(self.timeout)
            if frame:
                for client in self.clients:
                    if client.filter_cb(frame):
                        client.recv_cb(frame)

    def add_client(self, filter_cb, recv_cb):
        self.clients.append(Client(filter_cb, recv_cb))

    def stop(self):
        self.running = False
        if self.update_thread.is_alive():
            self.update_thread.join()

    def shutdown(self):
        self.stop()
        self.bus_manager.shutdown()

    def send(self, frame):
        """
        Send a frame by forwarding to the bus manager object
        """
        self.bus_manager.send(frame)


def init_router(bus_class, bus_params, logger=Logger("tinymovr"), timeout=0.1):
    """
    Initializes a bus router using a python-can bus instance
    """
    global bus_router
    if bus_router == None:
        bus_manager = BusManager(bus_class, bus_params, logger=logger)
        bus_router = BusRouter(bus_manager, timeout, logger=logger)
    else:
        logger.warning("Bus Router already exists")
    return bus_router


def destroy_router():
    """
    Destroys the existing bus router, stopping its thread.
    """
    global bus_router
    if bus_router != None:
        bus_router.shutdown()
        bus_router = None


def get_router():
    """
    Get the current bus router object
    """
    return bus_router


# Deprecated functions


def init_tee(bus, timeout=0.1):
    """
    Initializes a tee using a python-can bus instance
    """
    raise RuntimeError(
        "init_tee() is no longer supported. Please use init_router() instead."
    )


def destroy_tee():
    """
    Destroys the existing tee, stopping its thread.
    """
    warnings.warn(
        "destroy_tee() is deprecated and will be removed in future versions. Please use destroy_router() instead.",
        DeprecationWarning,
        stacklevel=2,
    )
    destroy_router()


def get_tee():
    """
    Get the current tee object
    """
    warnings.warn(
        "get_tee() is deprecated and will be removed in future versions. Please use get_router() instead.",
        DeprecationWarning,
        stacklevel=2,
    )
    get_router()
