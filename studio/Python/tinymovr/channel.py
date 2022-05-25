"""
Copyright 2022 Ioannis Chatzikonstantinou (Tinymovr)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
"""

import time
import can
from functools import cached_property
from tinymovr.constants import CAN_DEV_MASK, CAN_EP_SIZE, CAN_EP_MASK, CAN_SEQ_SIZE, CAN_SEQ_MASK
from tinymovr.codec import MultibyteCodec


class ResponseError(Exception):
    def __init__(self, kw, *args, **kwargs):
        msg = "Node {} did not respond".format(kw)
        super().__init__(msg, *args, **kwargs)
        self.kw = kw


class CANChannel:
    def __init__(self, node_id, bus):
        self.node_id = node_id
        self.bus = bus

    def send(self, data, ep_id):
        rtr = False if data and len(data) else True
        self.bus.send(self.create_frame(ep_id, rtr, data))

    def recv(self, ep_id, timeout=0.1):
        frame_id = arbitration_from_ids(ep_id, 0, self.node_id)
        frame = self._recv_frame(timeout=timeout)
        if frame.arbitration_id == frame_id:
            return frame.data
        else:
            e_ep_id, _, e_node_id = ids_from_arbitration(frame_id)
            s_ep_id, _, s_node_id = ids_from_arbitration(frame.arbitration_id)
            raise IOError(
                "Received id mismatch. Expected: Node: {}, Endpoint:{}; Got: Node: {}, Endpoint:{}".format(
                    e_node_id, e_ep_id, s_node_id, s_ep_id 
                )
            )

    def _recv_frame(self, timeout=0.1, sleep_interval=0.01):
        total_interval = 0
        while total_interval < timeout:
            frame = self.bus.recv(0)
            if frame:
                return frame
            time.sleep(sleep_interval)
            total_interval += sleep_interval
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


def ids_from_arbitration(arbitration_id):
    node_id = (arbitration_id & CAN_DEV_MASK) >> (CAN_EP_SIZE + CAN_SEQ_SIZE)
    seq_id = (arbitration_id & CAN_SEQ_MASK) >> CAN_EP_SIZE
    ep_id = arbitration_id & CAN_EP_MASK
    return ep_id, seq_id, node_id


def arbitration_from_ids(ep_id, seq_id, node_id):
    return (
        ep_id & CAN_EP_MASK | 
        ((seq_id << CAN_EP_SIZE) & CAN_SEQ_MASK) |
        ((node_id << (CAN_EP_SIZE + CAN_SEQ_SIZE)) & CAN_DEV_MASK)
    )
