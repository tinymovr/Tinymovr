import time
import math
from collections import deque
import can
import isotp
import msgpack
import avlos
from tinymovr.constants import ErrorIDs
from tinymovr.codec import MultibyteCodec
from tinymovr.iface.can_bus import create_frame, extract_node_message_id
from tinymovr.iface.can_bus import can_endpoints


AVLOS_SPEC_ADDR = 0xFFFF
AVLOS_CMD_GET = 0x01
AVLOS_CMD_SET = 0x02
ENC_TICKS = 8192
rad_to_ticks = ENC_TICKS / (2 * math.pi)


class SimBus(can.BusABC):
    """
    A Bus subclass that implements a Tinymovr
    controller in silico
    """

    def __init__(self, channel, can_filters=None, **kwargs):
        super().__init__(channel, can_filters, **kwargs)
        self.channel_info: str = "Tinymovr Test Channel"
        self.node_id = 1
        self.codec: MultibyteCodec = MultibyteCodec()

        self.rx_q = deque()
        self.tx_q = deque()
        self.sending = False
        
        # Mechanical and electrical properties
        self.Kv_SI: float = 10.0
        self.R: float = 0.05
        self.M: float = 0.5
        self.I: float = self.M * self.R * self.R  # thin hoop formula
        self.TICKS: int = ENC_TICKS

        # Spec
        self.spec = msgpack.packb([1, 2, 3], use_bin_type=True)

        # Endpoints
        self.endpoints = [self._get_version, self._get_pos_setpoint, self._set_pos_setpoint]

        # Initialize a second iso-tp layer that will 
        # correspond to the simulated mcu side
        self.layer = isotp.TransportLayer(
            self.isotp_rx, self.isotp_tx, address=self.node_id,
            error_handler=None, params=None)

    def process_frame(self):
        isotp_data = self.layer.recv()
        if isotp_data:
            return_data = self.process_isotp(isotp_data)
            if return_data:
                self.layer.send(return_data, self.node_id)

    def process_isotp(self, data):
        ep_idx = self.codec.deserialize(data[0:2])
        if AVLOS_SPEC_ADDR == ep_idx:
            return self.spec
        elif AVLOS_CMD_GET == data[2]:
            return self.endpoints[ep_idx]()
        elif AVLOS_CMD_SET == data[2]:
            return self.endpoints[ep_idx](data[2:])
        
    def isotp_rx(self):
        # This receives on the Tinymovr side
        try:
            return self.rx_q.pop()
        except IndexError:
            return None

    def isotp_tx(self, payload):
        # This transmits on the Tinymovr side
        self.tx_q.appendleft(payload)

    def send(self, msg):
        # This transmits on the PC SIDE
        self.rx_q.appendleft(msg)
        self.process_frame()

    def _recv_internal(self, timeout):
        # This receives on the PC SIDE
        # TODO: wait for timeout
        try:
            return self.tx_q.pop()
        except IndexError:
            # Wait and retry
            time.sleep(0.5)
            try:
                return self.tx_q.pop()
            except IndexError:
                return None


    # Bunch of definitions

    def _get_version(self):
        '''
        '''
        return 1

    def _get_pos_setpoint(self):
        '''
        '''
        return 1.5

    def _set_pos_setpoint(self, value):
        '''
        '''
        pass