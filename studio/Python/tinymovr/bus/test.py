import can
from time import sleep
from tinymovr import Endpoints
from tinymovr.iface import (
    create_frame, extract_node_message_id, CANBusCodec)


class Test(can.BusABC):

    def __init__(self, channel, can_filters=None, **kwargs):
        super().__init__(channel, can_filters, **kwargs)
        self.channel_info = "Tinymovr Test Channel"
        self.node_id = 1
        self.buffer: can.Message = None
        self.codec = CANBusCodec()

        self.endpoint_values = {
            k:self.codec.deserialize(bytearray(8), *v["types"])
            for k, v in Endpoints.items() if "types" in v
        }

    def send(self, msg: can.Message):
        arbitration_id: int = msg.arbitration_id
        node_id, msg_id = extract_node_message_id(arbitration_id)
        if node_id == self.node_id:
            for d in Endpoints.values():
                if d["ep_id"] == msg_id and "r" in d["type"]:
                    self.buffer = create_frame(
                        node_id, msg_id, False, 
                        self.codec.serialize(self.endpoint_values[msg_id], *d["types"])
                    )
                    break

    def _recv_internal(self, timeout: float) -> can.Message:
        if self.buffer:
            msg: can.Message = self.buffer
            self.buffer = None
            return msg, True
        if timeout > 0:
            sleep(timeout)
        return None, True
