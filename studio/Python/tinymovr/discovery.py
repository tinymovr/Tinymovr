"""
Tinymovr Discovery Module
Copyright Ioannis Chatzikonstantinou 2020-2023

Implements a class to discover nodes on the CAN bus based on
the reception of heartbeat frames

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
from threading import Thread, Event
from tinymovr.channel import ResponseError
from tinymovr.tee import get_tee
from tinymovr.constants import HEARTBEAT_BASE
from tinymovr.config import create_device_with_hash_msg, ProtocolVersionError


class Discovery:
    """
    Discover Tinymovr instances on the CAN bus using the periodically
    transmitted heartbeat frame.
    """

    def __init__(self, appeared_cb, disappeared_cb, logger, lost_timeout=2.0):
        self.logger = logger
        self.appeared_cb = appeared_cb
        self.disappeared_cb = disappeared_cb
        self.lost_timeout = lost_timeout
        self.queued_nodes = []
        self.thread = Thread(target=self._runner, daemon=True)
        self.evt = Event()
        self.thread.start()

        self.reset()

        get_tee().add(
            lambda msg: HEARTBEAT_BASE == msg.arbitration_id & HEARTBEAT_BASE,
            self._recv_cb,
        )

    def reset(self):
        self.active_nodes = {}
        self.update_stamps = {}
        self.pending_nodes = set()
        self.incompatible_nodes = set()

    def _recv_cb(self, frame):
        """
        Callback that processes a received message,
        initializing a new node.
        """
        now = time.time()
        node_id = frame.arbitration_id & 0x3F
        if node_id in self.incompatible_nodes:
            pass
        elif node_id in self.active_nodes:
            self.update_stamps[node_id] = now
        elif node_id not in self.pending_nodes:
            self.pending_nodes.add(node_id)
            try:
                node = create_device_with_hash_msg(frame)
                self.active_nodes[node_id] = node
                self.update_stamps[node_id] = now
                self._append_to_queue((node, node_id))
            except ResponseError as e:
                self.logger.error(e)
            except ProtocolVersionError as e:
                self.logger.error(e)
                self.incompatible_nodes.add(node_id)
            self.pending_nodes.remove(node_id)

    def _append_to_queue(self, node_info):
        """
        Appends a node to the queued nodes list
        """
        self.queued_nodes.append(node_info)
        self.evt.set()

    def _runner(self):
        """
        Asynchronously processes queued nodes
        """
        while True:
            self.evt.wait()
            try:
                self.appeared_cb(*(self.queued_nodes.pop()))
            except IndexError:
                self.evt.clear()
