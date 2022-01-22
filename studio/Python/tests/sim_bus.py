import time
import json
import math
import threading
from collections import deque

import can
import isotp
import msgpack
from avlos.codecs import MultibyteCodec
from avlos.types import DataType
from tinymovr.isotp_channel import CAN_EP_BITS


AVLOS_SPEC_ADDR = 0xFFFF
AVLOS_CMD_GET = 0x01
AVLOS_CMD_SET = 0x02
ENC_TICKS = 8192
rad_to_ticks = ENC_TICKS / (2 * math.pi)

TEST_TM_RX_ADDR = 0x3E
TEST_TM_TX_ADDR = 0x3F


class TestEndpoint:

    def __init__(self, getter, setter, codec, datatype=None):
        self.getter = getter
        self.setter = setter
        self.codec = codec
        self.datatype = datatype

    def process(self, trimmed_data):
        if AVLOS_CMD_GET == trimmed_data[0]:
            return self.codec.serialize([self.getter()], self.datatype)
        elif AVLOS_CMD_SET == trimmed_data[0]:
            v = self.codec.deserialize(trimmed_data[1:], self.datatype)[0]
            return self.setter(v)


class SimBus(can.BusABC):
    """
    A Bus subclass that implements a Tinymovr
    controller in silico
    """

    def __init__(self, channel, can_filters=None, node_id=1, **kwargs):
        super().__init__(channel, can_filters, **kwargs)
        self.channel_info: str = "Tinymovr Test Channel"
        self.node_id = node_id
        self.codec = MultibyteCodec()

        self.rx_q = deque()
        self.tx_q = deque()
        self.sending = False
        
        # Mechanical and electrical properties
        self.Kv_SI = 10.0
        self.r = 0.05
        self.M = 0.5
        self.I = self.M * self.r * self.r  # thin hoop formula
        self.R = 0.2
        self.L = 0.0005
        self.TICKS: int = ENC_TICKS

        # Live values
        self.pos_setpoint = 0.0
        self.vel_setpoint = 0.0

        # Spec
        self.spec = msgpack.packb(
            json.loads(
                """
            {
                "version": {"id":0, "K":"A", "T":2},
                "controller":{
                    "pos_setpoint": {"id":1, "K":"A", "T":10},
                    "vel_setpoint": {"id":2, "K":"A", "T":10}
                },
                "motor":{
                    "R": {"id":3, "K":"A", "T":10},
                    "L": {"id":4, "K":"A", "T":10}
                },
                "reset": {"id":5, "K":"F", "T":0}
            }
        """
            )
        )

        # Endpoints
        self.endpoints = [
            TestEndpoint(self._get_version, None, self.codec, DataType.UINT16),
            TestEndpoint(self._get_pos_setpoint, self._set_pos_setpoint, self.codec, DataType.FLOAT),
            TestEndpoint(self._get_vel_setpoint, self._set_vel_setpoint, self.codec, DataType.FLOAT),
            TestEndpoint(self._get_R, self._null_setter, self.codec, DataType.FLOAT),
            TestEndpoint(self._get_L, self._null_setter, self.codec, DataType.FLOAT),
            TestEndpoint(self._reset, None, self.codec)
        ]

        # Initialize a second iso-tp layer that will 
        # correspond to the simulated mcu side
        address = isotp.Address(isotp.AddressingMode.Normal_11bits,
                          rxid=(TEST_TM_RX_ADDR + (self.node_id << CAN_EP_BITS)),
                          txid=(TEST_TM_TX_ADDR + (self.node_id << CAN_EP_BITS)) )
        self.layer = isotp.TransportLayer(
            self.isotp_rx, self.isotp_tx, address=address, params=None)

        self.stop_requested = False
        self.update_thread = threading.Thread(target=self.isotp_update, daemon=True)
        self.update_thread.start()

    def isotp_update(self):
        while False == self.stop_requested:
            self.layer.process()
            time.sleep(self.layer.sleep_time())

    def process_frame(self):
        isotp_data = self.layer.recv()
        if isotp_data:
            return_data = self.process_isotp(isotp_data)
            if return_data:
                self.layer.send(return_data)

    def process_isotp(self, data):
        ep_idx = self.codec.deserialize(data[:2], DataType.UINT16)[0]
        if AVLOS_SPEC_ADDR == ep_idx:
            return self.spec
        try:
            ep = self.endpoints[ep_idx]
            return ep.process(data[2:])
        except IndexError:
            pass

    # -- Tinymovr Side
        
    def isotp_rx(self):
        # This receives on the Tinymovr side
        try:
            p = self.rx_q.pop()
            return p
        except IndexError:
            return None

    def isotp_tx(self, payload):
        # This transmits on the Tinymovr side
        self.tx_q.appendleft(payload)

    # -- PC Side

    def send(self, msg):
        # This transmits on the PC SIDE
        isotp_msg = isotp.CanMessage(
            arbitration_id=msg.arbitration_id, dlc=msg.dlc, data=msg.data)
        self.rx_q.appendleft(isotp_msg)
        self.layer.process()
        self.process_frame()

    def _recv_internal(self, timeout):
        # This receives on the PC SIDE
        # TODO: wait for timeout
        try:
            isotp_frame = self.tx_q.pop()
            can_frame = can.Message(
                0, arbitration_id=isotp_frame.arbitration_id, 
                is_extended_id=False,
                dlc=isotp_frame.dlc, 
                data=isotp_frame.data)
            return (can_frame, False)
        except IndexError:
            # Wait and retry
            time.sleep(0.02)
            try:
                return self.tx_q.pop()
            except IndexError:
                return (None, False)


    # Bunch of definitions

    def _get_version(self):
        return 1

    def _get_pos_setpoint(self):
        return self.pos_setpoint

    def _set_pos_setpoint(self, value):
        self.pos_setpoint = value

    def _get_vel_setpoint(self):
        return self.vel_setpoint

    def _get_R(self):
        return self.R

    def _get_L(self):
        return self.L

    def _set_vel_setpoint(self, value):
        self.vel_setpoint = value

    def _reset(self):
        self.pos_setpoint = 0.0
        self.vel_setpoint = 0.0

    def _null_setter(self, value):
        pass