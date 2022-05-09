"""
Copyright 2022 Ioannis Chatzikonstantinou (Tinymovr)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
"""

import time
import threading
import yaml
import importlib.resources
from tinymovr.codec import DataType
from tinymovr.channel import CANChannel, ResponseError
from tinymovr.tee import Tee
from tinymovr.constants import HEARTBEAT_BASE, CAN_EP_SIZE
from avlos.deserializer import deserialize


class ProtocolVersionError(Exception):
    pass


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
            self.logger.debug("Discovery iteration...")
            now = time.time()
            msg = self.tee.recv(0)
            while msg:
                node_id = msg.arbitration_id & 0x3F
                if node_id in self.active_nodes:
                    self.update_stamps[node_id] = now
                elif node_id not in self.pending_nodes:
                    self.pending_nodes.add(node_id)
                    tee = Tee(self.bus, lambda msg: msg.arbitration_id >> CAN_EP_SIZE & 0xFF == node_id)
                    chan = CANChannel(node_id, tee)
                    try:
                        device_hash_uint32, *_ = chan.serializer.deserialize(
                            msg.data[:4], DataType.UINT32
                        )
                        node = deserialize(self.dev_def)
                        if node.hash_uint32 != device_hash_uint32:
                            raise ProtocolVersionError(
                                "Incompatible protocol versions (hash mismatch).\nPlease try upgrading firmware & studio to the same version."
                            )
                        node._channel = chan
                        self.active_nodes[node_id] = node
                        self.update_stamps[node_id] = now
                        self.appeared_cb(node, node_id)
                    except ResponseError as e:
                        self.logger.error(e)
                    self.pending_nodes.remove(node_id)
                msg = self.tee.recv(0)

            # for_removal = set()
            # for node_id, stamp in self.update_stamps.items():
            #     if now - stamp > self.lost_timeout:
            #         for_removal.add(node_id)
            # for node_id in for_removal:
            #     del self.active_nodes[node_id]
            #     del self.update_stamps[node_id]
            #     self.disappeared_cb(node_id)
            time.sleep(0.1)
