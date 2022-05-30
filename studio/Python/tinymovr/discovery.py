"""
Tinymovr Discovery Module
Copyright Ioannis Chatzikonstantinou 2020-2022

Implements a class to discover nodes on the CAN bus

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
"""

import time
import threading
import yaml
import importlib.resources
from tinymovr.channel import CANChannel, ResponseError
from tinymovr.tee import Tee
from tinymovr.constants import HEARTBEAT_BASE, CAN_EP_SIZE
from tinymovr.config import create_device_with_hash_msg, ProtocolVersionError


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
        self.incompatible_nodes = set()

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
                if node_id in self.incompatible_nodes:
                    pass
                elif node_id in self.active_nodes:
                    self.update_stamps[node_id] = now
                elif node_id not in self.pending_nodes:
                    self.pending_nodes.add(node_id)
                    try:
                        node = create_device_with_hash_msg(msg, self.bus)
                        self.active_nodes[node_id] = node
                        self.update_stamps[node_id] = now
                        self.appeared_cb(node, node_id)
                    except ResponseError as e:
                        self.logger.error(e)
                    except ProtocolVersionError as e:
                        self.logger.error(e)
                        self.incompatible_nodes.add(node_id)
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
