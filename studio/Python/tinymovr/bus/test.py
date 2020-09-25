import can
from time import sleep
from datetime import datetime
from typing import Tuple, List, Dict, Union
from tinymovr.codec import MultibyteCodec
from tinymovr.iface.can import (create_frame, extract_node_message_id)
from tinymovr.iface.can import can_endpoints


class Test(can.BusABC):
    '''
    A Bus subclass that implements a Tinymovr
    controller in silico
    '''
    def __init__(self, channel, can_filters=None, **kwargs):
        super().__init__(channel, can_filters, **kwargs)
        self.channel_info: str = "Tinymovr Test Channel"
        self.node_id: int = 1
        self.buffer: can.Message = None
        self.codec: MultibyteCodec = MultibyteCodec()
        self.Kv: float = 100.
        self.CPR: int = 8192

        self.last_call_time = datetime.now()

        self.ep_func_map: Dict[int, callable] = {
            0x03: self._get_state,
            0x07: self._set_state,
            0x09: self._get_encoder_estimates,
            0x0A: self._get_setpoints,
            0x0C: self._set_pos_setpoint,
            0x0D: self._set_vel_setpoint,
            0x0E: self._set_cur_setpoint,
            0x1A: self._get_device_info
        }

        self._state: Dict[str, Union[int, float]] = {
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
        diff = (time_now - self.last_call_time).total_seconds()
        self.last_call_time = time_now

        if self._state["state"] == 2:
            self._state["current_estimate"] = self._state["current_setpoint"]
            if self._state["mode"] == 0:
                torque = self._state["current_estimate"] / self.Kv
                v: float = self._state["velocity_estimate"]
                self._state["velocity_estimate"] += torque * self.CPR * diff
                self._state["position_estimate"] += (v + 0.5 * torque * self.CPR * diff) * diff
            elif self._state["mode"] == 1:
                self._state["current_estimate"] = 0
                v: float = self._state["velocity_estimate"]
                self._state["velocity_estimate"] = self._state["velocity_setpoint"]
                self._state["position_estimate"] += v * diff
            elif self._state["mode"] == 2:
                self._state["current_estimate"] = 0
                self._state["velocity_estimate"] = 0
                self._state["position_estimate"] = self._state["position_setpoint"]
        if self._state["state"] == 0:
            self._state["current_estimate"] = 0
            self._state["velocity_estimate"] = 0

    # ---- Endpoint methods -----------------------------------------

    def _get_state(self, payload):
        vals: Tuple = (self._state["error"],
                       self._state["state"],
                       self._state["mode"])
        gen_payload = self.codec.serialize(
            vals, *can_endpoints["state"]["types"])
        self.buffer = create_frame(self.node_id, 0x03, False, gen_payload)

    def _set_state(self, payload):
        vals = self.codec.deserialize(
            payload, *can_endpoints["set_state"]["types"])
        self._state["state"] = vals[0]
        self._state["mode"] = vals[1]

    def _get_device_info(self, payload):
        vals: Tuple = (0, 0, 7, 1, 25)
        gen_payload = self.codec.serialize(
            vals, *can_endpoints["device_info"]["types"])
        self.buffer = create_frame(self.node_id, 0x1A, False, gen_payload)

    def _get_encoder_estimates(self, payload):
        vals: Tuple = (self._state["position_estimate"],
                       self._state["velocity_estimate"])
        gen_payload = self.codec.serialize(
            vals, *can_endpoints["encoder_estimates"]["types"])
        self.buffer = create_frame(self.node_id, 0x09, False, gen_payload)

    def _get_setpoints(self, payload):
        vals: Tuple = (self._state["position_setpoint"],
                       self._state["velocity_setpoint"])
        gen_payload = self.codec.serialize(
            vals, *can_endpoints["setpoints"]["types"])
        self.buffer = create_frame(self.node_id, 0x0A, False, gen_payload)

    def _set_pos_setpoint(self, payload):
        vals: List = self.codec.deserialize(
            payload, *can_endpoints["set_pos_setpoint"]["types"])
        self._state["position_setpoint"] = vals[0]
        self._state["velocity_setpoint"] = vals[1]
        self._state["current_setpoint"] = vals[2]

    def _set_vel_setpoint(self, payload):
        vals: List = self.codec.deserialize(
            payload, *can_endpoints["set_vel_setpoint"]["types"])
        self._state["velocity_setpoint"] = vals[0]
        self._state["current_setpoint"] = vals[1]

    def _set_cur_setpoint(self, payload):
        vals: List = self.codec.deserialize(
            payload, *can_endpoints["set_cur_setpoint"]["types"])
        self._state["current_setpoint"] = vals[0]
