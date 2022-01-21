
import time
import threading

from avlos import get_object_tree
from tinymovr.isotp_channel import ISOTPChannel, ResponseError
from tinymovr.tee import Tee


HEARTBEAT_BASE = 0x700


class Discovery:
    '''
    Discover Tinymovr instances on the CAN bus using the periodically
    transmitted heartbeat frame.
    '''
    def __init__(self, can_bus, appeared_cb, disappeared_cb, logger, lost_timeout = 2.0):
        self.can_bus = can_bus
        self.logger = logger
        self.appeared_cb = appeared_cb
        self.disappeared_cb = disappeared_cb
        self.lost_timeout = lost_timeout

        self.isotp_channels = {}
        self.nodes = {}
        self.update_stamps = {}

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
            now = time.time()
            msg = self.tee.recv()
            while msg:
                node_id = msg.arbitration_id & 0x3F
                if node_id in self.nodes:
                    self.update_stamps[node_id] = now
                if node_id not in self.nodes:
                    chan = ISOTPChannel(self.can_bus, node_id, self.logger)
                    self.isotp_channels[node_id] = chan
                    try:
                        node = get_object_tree(chan)
                        self.nodes[node_id] = node
                        self.update_stamps[node_id] = now
                        self.appeared_cb(node, node_id)
                    except ResponseError as e:
                        self.logger.error(e)
                msg = self.tee.recv()
            
            # for_removal = set()
            # for node_id, stamp in self.update_stamps.items():
            #     if now - stamp > self.lost_timeout:
            #         for_removal.add(node_id)
            # for node_id in for_removal:
            #     del self.nodes[node_id]
            #     del self.isotp_channels[node_id]
            #     del self.update_stamps[node_id]
            #     self.disappeared_cb(node_id)
            time.sleep(0.5)
