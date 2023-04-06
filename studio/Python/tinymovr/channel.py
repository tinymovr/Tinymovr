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
from tinymovr.tee import get_tee
from tinymovr.constants import (
    CAN_DEV_MASK,
    CAN_EP_SIZE,
    CAN_EP_MASK,
    CAN_SEQ_SIZE,
    CAN_SEQ_MASK,
)
from tinymovr.codec import MultibyteCodec


class ResponseError(Exception):
    def __init__(self, kw, *args, **kwargs):
        msg = "Node {} did not respond".format(kw)
        super().__init__(msg, *args, **kwargs)
        self.kw = kw


class CANChannel(BaseChannel):
    def __init__(self, node_id):
        self.node_id = node_id
        get_tee().add(
            lambda frame: frame.is_remote_frame == False
            and ids_from_arbitration(frame.arbitration_id)[2] == node_id,
            self._recv_cb,
        )
        self.queue = []
        self.lock = Lock()
        self.evt = Event()

    def _recv_cb(self, frame):
        self.queue.append(frame)
        self.evt.set()

    def send(self, data, ep_id):
        rtr = False if data and len(data) else True
        get_tee().send(self.create_frame(ep_id, rtr, data))

    def recv(self, ep_id, timeout=1.0):
        with self.lock:
            if not self.evt.wait(timeout=timeout):
                print("missed")
            self.evt.clear()
            frame_id = arbitration_from_ids(ep_id, 0, self.node_id)
            index = 0
            while index < len(self.queue):
                if self.queue[index].arbitration_id == frame_id:
                    return self.queue.pop(index).data
                index += 1  
            raise ResponseError(self.node_id)
        
    def create_frame(self, endpoint_id, rtr=False, payload=None):
        """
        Generate a CAN frame using python-can Message class
        """
        return can.Message(
            arbitration_id=arbitration_from_ids(endpoint_id, 0, self.node_id),
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
    Generate endpoint, message sequence and node ids
    from a CAN arbitration id
    """
    node_id = (arbitration_id & CAN_DEV_MASK) >> (CAN_EP_SIZE + CAN_SEQ_SIZE)
    seq_id = (arbitration_id & CAN_SEQ_MASK) >> CAN_EP_SIZE
    ep_id = arbitration_id & CAN_EP_MASK
    return ep_id, seq_id, node_id


def arbitration_from_ids(ep_id, seq_id, node_id):
    """
    Generate a CAN arbitration id from endpoint,
    message sequence and node ids
    """
    return (
        ep_id & CAN_EP_MASK
        | ((seq_id << CAN_EP_SIZE) & CAN_SEQ_MASK)
        | ((node_id << (CAN_EP_SIZE + CAN_SEQ_SIZE)) & CAN_DEV_MASK)
    )
