
import time
import threading

from avlos import get_object_tree
from tinymovr.isotp_channel import ISOTPChannel
from tinymovr.tee import Tee


HEARTBEAT_BASE = 0x700


class Discovery:
    '''
    Discover Tinymovr instances on the CAN bus using the periodically
    transmitted heartbeat frame.
    '''
    def __init__(self, can_bus, appeared_cb, disappeared_cb, logger):
        self.can_bus = can_bus
        self.logger = logger
        self.appeared_cb = appeared_cb
        self.disappeared_cb = disappeared_cb

        self.isotp_channels = {}
        self.nodes = {}

        self.tee = Tee(can_bus, lambda msg: HEARTBEAT_BASE == msg.arbitration_id & HEARTBEAT_BASE)

        self.update_thread = threading.Thread(target=self.update, daemon=True)
        self.update_thread.start()

    def update(self):
        '''
        Periodically process pending messages.

        Note that this will receive only filtered messages from the
        correct tee, as such will not disrupt other receivers.
        '''
        while True:
            msg = self.tee.recv()
            while msg:
                node_id = msg.arbitration_id & 0x3F
                if node_id not in self.nodes:
                    chan = ISOTPChannel(self.can_bus, node_id, self.logger)
                    self.isotp_channels[node_id] = chan
                    node = get_object_tree(chan)
                    self.nodes[node_id] = node
                    self.appeared_cb(node, node_id)
                msg = self.tee.recv()
            time.sleep(0.5)
