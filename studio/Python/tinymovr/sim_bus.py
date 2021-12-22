import time
import math
from collections import deque
import can
import isotp
import pkg_resources
from time import sleep
from datetime import datetime
from typing import Tuple, List, Dict, Union
from tinymovr.constants import ErrorIDs
from tinymovr.codec import MultibyteCodec
from tinymovr.iface.can_bus import create_frame, extract_node_message_id
from tinymovr.iface.can_bus import can_endpoints


ENC_TICKS: int = 8192
rad_to_ticks: float = ENC_TICKS / (2 * math.pi)


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

        # Initialize a second iso-tp layer that will 
        # correspond to the simulated mcu side
        self.layer = isotp.TransportLayer(
            self.isotp_rx, self.isotp_tx, address=self.node_id,
            error_handler=None, params=None)

    def process_frame(self):
        isotp_frame = self.layer.recv()
        if isotp_frame:
            data = bytearray()
            self.layer.send(data, self.node_id)

        
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


