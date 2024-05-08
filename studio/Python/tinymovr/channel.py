"""
Tinymovr CAN Channel Module
Copyright Ioannis Chatzikonstantinou 2020-2023

Implements a CAN bus communications channel

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

from threading import Lock, Event
import can
from functools import cached_property
from avlos.channel import BaseChannel
from tinymovr.bus_router import get_router
from tinymovr.constants import (
    CAN_DEV_MASK,
    CAN_EP_SIZE,
    CAN_EP_MASK,
    CAN_HASH_SIZE,
    CAN_HASH_MASK,
)
from tinymovr.codec import MultibyteCodec


class ResponseError(Exception):
    def __init__(self, node_id):
        super().__init__(f"Node {node_id} did not respond")
        self.node_id = node_id


class CANChannel(BaseChannel):
    def __init__(self, node_id, compare_hash = 0):
        self.node_id = node_id
        self.compare_hash = compare_hash
        self.queue = []
        self.lock = Lock()
        self.evt = Event()
        get_router().add_client(self._filter_frame, self._recv_cb)

    def _filter_frame(self, frame):
        return not frame.is_remote_frame and ids_from_arbitration(frame.arbitration_id)[2] == self.node_id

    def _recv_cb(self, frame):
        """
        Callback method for receiving frames. Append incoming frame
        to the queue and set the event flag.
        """
        self.queue.append(frame)
        self.evt.set()

    def send(self, data, ep_id):
        """
        Send a CAN frame to a specific endpoint. The `rtr` flag is set
        based on whether data is provided, and the frame is sent via the
        global bus router instance.
        """
        rtr = False if data and len(data) else True
        get_router().send(self.create_frame(ep_id, rtr, data))

    def recv(self, ep_id, timeout=1.0):
        """
        Receive a CAN frame from a specific endpoint. This method waits
        for a frame with a matching arbitration ID, removes it from the
        queue, and returns the frame data. If no matching frame is found
        within the specified timeout, a ResponseError is raised.
        """
        with self.lock:
            self.evt.wait(timeout=timeout)
            self.evt.clear()
            for frame in self.queue:
                inc_ep_id, inc_hash, _ = ids_from_arbitration(frame.arbitration_id)
                if inc_ep_id == ep_id and (inc_hash == self.compare_hash or inc_hash == 0):
                    self.queue.remove(frame)
                    return frame.data
            raise ResponseError(self.node_id)

    def create_frame(self, endpoint_id, rtr=False, payload=None):
        """
        Generate a CAN frame using python-can Message class
        """
        return can.Message(
            arbitration_id=arbitration_from_ids(endpoint_id, self.compare_hash, self.node_id),
            is_extended_id=True,
            is_remote_frame=rtr,
            data=payload,
        )

    @cached_property
    def serializer(self):
        return MultibyteCodec()


# TODO: Implement unit test for these functions
def ids_from_arbitration(arbitration_id):
    """
    Generate endpoint, hash value and node ids
    from a CAN arbitration id
    """
    node_id = (arbitration_id & CAN_DEV_MASK) >> (CAN_EP_SIZE + CAN_HASH_SIZE)
    hash = (arbitration_id & CAN_HASH_MASK) >> CAN_EP_SIZE
    ep_id = arbitration_id & CAN_EP_MASK
    return ep_id, hash, node_id


def arbitration_from_ids(ep_id, hash, node_id):
    """
    Generate a CAN arbitration id from endpoint,
    hash value and node ids
    """
    return (
        ep_id & CAN_EP_MASK
        | ((hash << CAN_EP_SIZE) & CAN_HASH_MASK)
        | ((node_id << (CAN_EP_SIZE + CAN_HASH_SIZE)) & CAN_DEV_MASK)
    )
