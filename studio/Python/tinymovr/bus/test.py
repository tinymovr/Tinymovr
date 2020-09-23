import can
from time import sleep
from datetime import datetime
from tinymovr import Endpoints
from tinymovr.iface import (
    create_frame, extract_node_message_id, CANBusCodec)


class Test(can.BusABC):
    '''
    A Bus subclass that implements a Tinymovr
    controller in silico
    '''
    def __init__(self, channel, can_filters=None, **kwargs):
        super().__init__(channel, can_filters, **kwargs)
        self.channel_info = "Tinymovr Test Channel"
        self.node_id = 1
        self.buffer: can.Message = None
        self.codec = CANBusCodec()
        self.Kv = 100

        self.last_call_time = datetime.now()

        self.ep_func_map = {
            0x03: self._get_state,
            0x09: self._get_encoder_estimates,
            0x0A: self._get_setpoints,
            0x0C: self._set_pos_setpoint,
            0x1A: self._get_device_info
        }

        self._state = {
            "error": 0,
            "state": 0,
            "mode": 0,
            "position_estimate": 0,
            "velocity_estimate": 0,
            "current_estimate": 0,
            "position_setpoint": 0,
            "velocity_setpoint": 0,
            "current_setpoint": 0
        }

    def send(self, msg: can.Message):
        self._update_state()
        arbitration_id: int = msg.arbitration_id
        node_id, msg_id = extract_node_message_id(arbitration_id)
        if node_id == self.node_id:
            try:
                self.ep_func_map[msg_id](msg.data)
            except KeyError:
                pass

    def _recv_internal(self, timeout: float) -> can.Message:
        self._update_state()
        if self.buffer:
            msg: can.Message = self.buffer
            self.buffer = None
            return msg, True
        if timeout > 0:
            sleep(timeout)
        return None, True

    def _update_state(self):
        time_now = datetime.now()
        # diff = time_now - self.last_call_time
        self.last_call_time = time_now

        self._state["position_estimate"] = self._state["position_setpoint"]
        self._state["velocity_estimate"] = self._state["velocity_setpoint"]
        self._state["current_estimate"] = self._state["current_setpoint"]

    # ---- Endpoint methods -----------------------------------------

    def _get_state(self, payload):
        vals = (self._state["error"],
                self._state["state"],
                self._state["mode"])
        gen_payload = self.codec.serialize(vals,
                                           *Endpoints["state"]["types"])
        self.buffer = create_frame(self.node_id, 0x03, False, gen_payload)

    def _get_device_info(self, payload):
        vals = (0, 0, 7, 1, 25)
        gen_payload = self.codec.serialize(vals,
                                           *Endpoints["device_info"]["types"])
        self.buffer = create_frame(self.node_id, 0x1A, False, gen_payload)

    def _get_encoder_estimates(self, payload):
        vals = (self._state["position_estimate"],
                self._state["velocity_estimate"])
        gen_payload = self.codec.serialize(vals,
                                           *Endpoints["encoder_estimates"]["types"])
        self.buffer = create_frame(self.node_id, 0x09, False, gen_payload)

    def _get_setpoints(self, payload):
        vals = (self._state["position_setpoint"],
                self._state["velocity_setpoint"])
        gen_payload = self.codec.serialize(vals,
                                           *Endpoints["setpoints"]["types"])
        self.buffer = create_frame(self.node_id, 0x0A, False, gen_payload)

    def _set_pos_setpoint(self, payload):
        vals = self.codec.deserialize(payload,
                                      *Endpoints["set_pos_setpoint"]["types"])
        self._state["position_setpoint"] = vals[0]
        self._state["velocity_setpoint"] = vals[1]
        self._state["current_setpoint"] = vals[2]
