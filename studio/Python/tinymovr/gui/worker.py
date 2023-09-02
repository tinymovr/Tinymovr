"""
Tinymovr GUI Worker
Copyright Ioannis Chatzikonstantinou 2020-2023

The GUI Worker class, subclass of QObject, that handles
updates and IO, running in a separate thread

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
import can
from PySide6 import QtCore
from PySide6.QtCore import QObject
from PySide6.QtWidgets import (
    QApplication,
)
from tinymovr.gui import TimedGetter, RateLimitedFunction, get_dynamic_attrs
from tinymovr.tee import init_tee, destroy_tee
from tinymovr.discovery import Discovery
from tinymovr.constants import base_node_name


class Worker(QObject):

    update_attrs = QtCore.Signal(dict)
    regen = QtCore.Signal(dict)
    handle_error = QtCore.Signal(object)

    def __init__(self, busparams, logger):
        super().__init__()
        self.logger = logger
        self.mutx = QtCore.QMutex()
        init_tee(can.Bus(**busparams))
        self._init_containers()
        self.dsc = Discovery(self._device_appeared, self._device_disappeared, self.logger)
        self.timed_getter = TimedGetter()
        self._rate_limited_update = RateLimitedFunction(lambda: self._update(), 0.040)
        self.running = True

    def run(self):
        while self.running:
            self._rate_limited_update()  # calls update()
        destroy_tee()

    @QtCore.Slot()
    def stop(self):
        self.running = False

    def force_regen(self):
        self.mutx.lock()
        self.regen.emit(dict(self.devices_by_name))
        self.mutx.unlock()

    def reset(self):
        self.mutx.lock()
        self.dsc.reset()
        self._init_containers()
        self.regen.emit(dict(self.devices_by_name))
        self.mutx.unlock()

    @QtCore.Slot(dict)
    def update_active_attrs(self, d):
        if d["checked"] == True:
            self.active_attrs.add(d["attr"])
        else:
            self.active_attrs.discard(d["attr"])

    def _get_attr_values(self):
        vals = {}
        for attr in self.active_attrs:
            try:
                vals[attr.full_name] = self.timed_getter.get_value(attr.get_value)
            except Exception as e:
                self.handle_error.emit(e)
        start_time = time.time()
        self.dynamic_attrs.sort(
            key=lambda attr: self.dynamic_attrs_last_update[attr.full_name]
            if attr.full_name in self.dynamic_attrs_last_update
            else 0
        )
        for attr in self.dynamic_attrs:
            t = (
                self.dynamic_attrs_last_update[attr.full_name]
                if attr.full_name in self.dynamic_attrs_last_update
                else 0
            )
            if (attr.full_name not in vals) and (start_time - t > 0.5):
                try:
                    vals[attr.full_name] = self.timed_getter.get_value(attr.get_value)
                    self.dynamic_attrs_last_update[attr.full_name] = start_time
                except Exception as e:
                    self.handle_error.emit(e)
                break
        return vals

    def _init_containers(self):
        self.active_attrs = set()
        self.dynamic_attrs = []
        self.devices_by_name = {}
        self.names_by_id = {}
        self.dynamic_attrs_last_update = {}

    def _update(self):
        self.mutx.lock()
        last_updated = self._get_attr_values()
        if len(last_updated) > 0:
            self.update_attrs.emit(last_updated)
        self.mutx.unlock()
        QApplication.processEvents()

    def _device_appeared(self, device, node_id):
        self.mutx.lock()
        display_name = "{}{}".format(device.name, node_id)
        self.devices_by_name[display_name] = device
        self.names_by_id[node_id] = display_name
        device.name = display_name
        device.include_base_name = True
        self.dynamic_attrs = get_dynamic_attrs(self.devices_by_name)
        self.regen.emit(dict(self.devices_by_name))
        self.mutx.unlock()

    def _device_disappeared(self, node_id):
        self.mutx.lock()
        display_name = "{}{}".format(self.names_by_id[node_id], node_id)
        del self.devices_by_name[display_name]
        del self.names_by_id[node_id]
        self.dynamic_attrs = get_dynamic_attrs(self.devices_by_name)
        self.regen.emit(dict(self.devices_by_name))
        self.mutx.unlock()
