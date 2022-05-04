import time
import threading
import yaml
import importlib.resources
from tinymovr.channel import CANBusChannel, ResponseError
from tinymovr.tee import Tee
from avlos.deserializer import deserialize

HEARTBEAT_BASE = 0x700


class Discovery:
    """
    Discover Tinymovr instances on the CAN bus using the periodically
    transmitted heartbeat frame.
    """

    def __init__(self, bus, appeared_cb, disappeared_cb, logger, lost_timeout=2.0):
        self.bus = bus
        self.logger = logger
        self.appeared_cb = appeared_cb
        self.disappeared_cb = disappeared_cb
        self.lost_timeout = lost_timeout

        self.active_nodes = {}
        self.update_stamps = {}
        self.pending_nodes = set()

        self.tee = Tee(
            bus, lambda msg: HEARTBEAT_BASE == msg.arbitration_id & HEARTBEAT_BASE
        )

        def_path_str = str(
            importlib.resources.files("tinymovr").joinpath("config/device.yaml")
        )
        with open(def_path_str) as dev_def_raw:
            self.dev_def = yaml.safe_load(dev_def_raw)

        self.update_thread = threading.Thread(target=self.update, daemon=True)
        self.update_thread.start()

    def update(self):
        """
        Periodically process pending messages.

        Note that this will receive only filtered messages from the
        correct tee, as such will not disrupt other receivers.
        """
        while True:
            now = time.time()
            msg = self.tee.recv()
            while msg:
                node_id = msg.arbitration_id & 0x3F
                if node_id in self.active_nodes:
                    self.update_stamps[node_id] = now
                elif node_id not in self.pending_nodes:
                    self.pending_nodes.add(node_id)
                    tee = Tee(self.bus, lambda msg: msg.arbitration_id == node_id)
                    chan = CANBusChannel(node_id, tee)
                    try:
                        node = deserialize(self.dev_def)
                        node.set_channel(chan)
                        self.active_nodes[node_id] = node
                        self.update_stamps[node_id] = now
                        self.appeared_cb(node, node_id)
                    except ResponseError as e:
                        self.logger.error(e)
                    self.pending_nodes.remove(node_id)
                msg = self.tee.recv()

            # for_removal = set()
            # for node_id, stamp in self.update_stamps.items():
            #     if now - stamp > self.lost_timeout:
            #         for_removal.add(node_id)
            # for node_id in for_removal:
            #     del self.active_nodes[node_id]
            #     del self.update_stamps[node_id]
            #     self.disappeared_cb(node_id)
            time.sleep(0.1)
