import can
from time import sleep
from datetime import datetime
from tinymovr import Endpoints
from tinymovr.iface import (
    create_frame, extract_node_message_id, CANBusCodec)


class Test(can.BusABC):
    '''
    A Bus subclass to test Tinymovr Studio features offline
    '''
    def __init__(self, channel, can_filters=None, **kwargs):
        super().__init__(channel, can_filters, **kwargs)
        self.channel_info = "Tinymovr Test Channel"
        self.node_id = 1
        self.buffer: can.Message = None
        self.codec = CANBusCodec()

        self.last_call = datetime.now()

        self.ep_func_map = {
            0x1A: self._device_info
        }

    def send(self, msg: can.Message):
        arbitration_id: int = msg.arbitration_id
        node_id, msg_id = extract_node_message_id(arbitration_id)
        if node_id == self.node_id:
            try:
                self.ep_func_map[msg_id](msg.data)
            except KeyError:
                pass

    def _recv_internal(self, timeout: float) -> can.Message:
        if self.buffer:
            msg: can.Message = self.buffer
            self.buffer = None
            return msg, True
        if timeout > 0:
            sleep(timeout)
        return None, True

    def _device_info(self, payload):
        vals = (0, 0, 7, 1, 25)
        gen_payload = self.codec.serialize(vals,
                                           *Endpoints["device_info"]["types"])
        self.buffer = create_frame(self.node_id, 0x1A, False, gen_payload)
