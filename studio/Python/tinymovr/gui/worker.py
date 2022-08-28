import time
import can
from PySide2 import QtCore
from PySide2.QtCore import QObject
from PySide2.QtWidgets import (
    QApplication,
)
from tinymovr.tee import init_tee, destroy_tee
from tinymovr.discovery import Discovery
from tinymovr.constants import base_node_name


class Worker(QObject):

    update_attrs = QtCore.Signal(dict)
    regen = QtCore.Signal(dict)

    def __init__(self, bustype, channel, bitrate, logger):
        super().__init__()
        self.logger = logger

        init_tee(can.Bus(bustype=bustype, channel=channel, bitrate=bitrate))
        self.dsc = Discovery(
            self.node_appeared, self.node_disappeared, self.logger
        )
        self.active_attrs = []
        self.tms_by_id = {}
        self.running = True

    def run(self):
        while self.running:
            self.get_values()
            QApplication.processEvents()
            time.sleep(0.05)
        destroy_tee()

    @QtCore.Slot()
    def stop(self):
        self.running = False

    def get_values(self):
        # TODO: Handle possible exception
        updated_attrs = {attr.full_name: attr.get_value() for attr in self.active_attrs}
        if len(updated_attrs) > 0:
            self.update_attrs.emit(updated_attrs)

    def node_appeared(self, node, name):
        node_name = "{}{}".format(base_node_name, name)
        self.tms_by_id[node_name] = node
        self.regen.emit(self.tms_by_id)

    def node_disappeared(self, name):
        node_name = "{}{}".format(base_node_name, name)
        del self.tms_by_id[node_name]
        self.regen.emit(self.tms_by_id)

    @QtCore.Slot(dict)
    def update_checkstates(self, d):
        attr = d["attr"]
        if d["enabled"] == True and attr not in self.active_attrs:
            self.active_attrs.append(attr)
        elif d["enabled"] == False and attr in self.active_attrs:
            self.active_attrs.remove(attr)