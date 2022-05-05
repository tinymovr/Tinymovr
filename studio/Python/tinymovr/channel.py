"""
Copyright 2022 Ioannis Chatzikonstantinou (Tinymovr)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
"""

import can
from functools import cached_property
from tinymovr.constants import CAN_EP_BITS, CAN_EP_SIZE, CAN_EP_MASK
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
        frame_id = self.make_arbitration_id(ep_id)
        frame = self.bus.recv(timeout=timeout)
        if frame:
            if frame.arbitration_id == frame_id:
                return frame.data
            else:
                error_data = extract_node_message_id(frame_id)
                error_data += extract_node_message_id(frame.arbitration_id)
                raise IOError(
                    "Received id mismatch. Expected: Node: {}, Endpoint:{}; Got: Node: {}, Endpoint:{}".format(
                        *[hex(v) for v in error_data]
                    )
                )
        else:
            raise TimeoutError()

    def make_arbitration_id(self, endpoint_id):
        """
        Generates a CAN id from node and endpoint ids
        """
        return (self.node_id << CAN_EP_BITS) | endpoint_id

    def create_frame(self, endpoint_id, rtr=False, payload=None):
        """
        Generate a CAN frame using python-can Message class
        """
        return can.Message(
            arbitration_id=self.make_arbitration_id(endpoint_id),
            is_extended_id=False,
            is_remote_frame=rtr,
            data=payload,
        )

    @cached_property
    def serializer(self):
        return MultibyteCodec()


def extract_node_message_id(arbitration_id):
    node_id = arbitration_id >> CAN_EP_SIZE & 0xFF
    message_id = arbitration_id & CAN_EP_MASK
    return node_id, message_id
