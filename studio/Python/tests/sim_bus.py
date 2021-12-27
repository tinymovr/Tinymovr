import time
import json
import math
import threading
from collections import deque
import can
import isotp
import msgpack
import avlos
from tinymovr.constants import ErrorIDs
from avlos.codecs import MultibyteCodec
from avlos.types import DataType
from tinymovr.isotp_channel import create_frame, ISOTP_RX_ADDR, ISOTP_TX_ADDR, CAN_EP_BITS


AVLOS_SPEC_ADDR = 0xFFFF
AVLOS_CMD_GET = 0x01
AVLOS_CMD_SET = 0x02
ENC_TICKS = 8192
rad_to_ticks = ENC_TICKS / (2 * math.pi)

TEST_TM_RX_ADDR = 0x3E
TEST_TM_TX_ADDR = 0x3F

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
        self.Kv_SI: float = 10.0
        self.R: float = 0.05
        self.M: float = 0.5
        self.I: float = self.M * self.R * self.R  # thin hoop formula
        self.TICKS: int = ENC_TICKS

        # Live values
        self.pos_setpoint = 0.0
        self.vel_setpoint = 0.0

        # Spec
        self.spec = msgpack.packb(
            json.loads(
                """
            {
                "system":
                {
                    "version": {"id":0, "K":"A", "T":2},
                    "controller":{
                        "pos_setpoint": {"id":1, "K":"A", "T":10},
                        "vel_setpoint": {"id":2, "K":"A", "T":10}
                    },
                    "reset": {"id":3, "K":"F", "T":0}
                }
            }
        """
            )
        )

        # Endpoints
        self.getters = [self._get_version, self._get_pos_setpoint, self._get_vel_setpoint, self._reset]
        self.setters = [None, self._set_pos_setpoint, self._set_vel_setpoint, None]

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
        elif AVLOS_CMD_GET == data[2]:
            return self.codec.serialize([self.getters[ep_idx]()], DataType.FLOAT)
        elif AVLOS_CMD_SET == data[2]:
            v = self.codec.deserialize(data[3:], DataType.FLOAT)[0]
            return self.setters[ep_idx](v)

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
        '''
        '''
        return 1

    def _get_pos_setpoint(self):
        '''
        '''
        return self.pos_setpoint

    def _set_pos_setpoint(self, value):
        '''
        '''
        self.pos_setpoint = value

    def _get_vel_setpoint(self):
        '''
        '''
        return self.vel_setpoint

    def _set_vel_setpoint(self, value):
        '''
        '''
        self.vel_setpoint = value

    def _reset(self):
        '''
        '''
        self.pos_setpoint = 0.0
        self.vel_setpoint = 0.0