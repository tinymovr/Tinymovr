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
from tinymovr.channel import ResponseError
from tinymovr.gui import TimedGetter, get_dynamic_attrs
from tinymovr.bus_router import init_router, destroy_router
from tinymovr.device_discovery import DeviceDiscovery
from tinymovr.constants import base_node_name


class Worker(QObject):
    updateAttrsSignal = QtCore.Signal(dict)
    updateTimingsSignal = QtCore.Signal(dict)
    regenSignal = QtCore.Signal(dict)
    handleErrorSignal = QtCore.Signal(object)

    def __init__(self, bus_params, logger, bus_class=can.Bus, refresh_period=0.3):
        super().__init__()
        self.logger = logger
        self.mutx = QtCore.QMutex()
        #TODO: router init should not happen in GUI worker
        init_router(bus_class, bus_params, logger=logger)
        self._init_containers()
        self.dsc = DeviceDiscovery(
            self._device_appeared, self._device_disappeared, self.logger
        )
        self.timed_getter = TimedGetter()

        self.refresh_period = refresh_period

        self.dt_update = 1
        self.dt_load = 0
        self.t_last_update = time.time()

        self.visible_attrs = set()

    @QtCore.Slot()
    def stop(self):
        destroy_router()

    def force_regen(self):
        self.mutx.lock()
        self.regenSignal.emit(dict(self.devices_by_name))
        self.mutx.unlock()

    def reset(self):
        self.logger.info("Resetting Studio...")
        self.mutx.lock()
        self.dsc.reset()
        self._init_containers()
        self.regenSignal.emit(dict(self.devices_by_name))
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
                self.handleErrorSignal.emit(e)
        start_time = time.time()
        attrs_to_update = [attr for attr in self.dynamic_attrs if attr.full_name in self.visible_attrs]
        attrs_to_update.sort(
            key=lambda attr: self.dynamic_attrs_last_update[attr.full_name]
            if attr.full_name in self.dynamic_attrs_last_update
            else 0
        )
        for attr in attrs_to_update:
            t = (
                self.dynamic_attrs_last_update[attr.full_name]
                if attr.full_name in self.dynamic_attrs_last_update
                else 0
            )
            if (attr.full_name not in vals) and (start_time - t > self.refresh_period):
                try:
                    vals[attr.full_name] = self.timed_getter.get_value(attr.get_value)
                    self.dynamic_attrs_last_update[attr.full_name] = start_time
                except Exception as e:
                    self.handleErrorSignal.emit(e)
                break
        return vals
    
    @QtCore.Slot(set)
    def update_visible_attrs(self, attrs):
        """
        Update the set of visible attributes based on the signal from the main window.
        """
        self.visible_attrs = attrs

    def _init_containers(self):
        self.active_attrs = set()
        self.dynamic_attrs = []
        self.devices_by_name = {}
        self.names_by_id = {}
        self.dynamic_attrs_last_update = {}

    def _update(self):
        t = time.time()
        self.dt_update = self.dt_update * 0.95 + (t - self.t_last_update) * 0.05
        self.t_last_update = t
        self.mutx.lock()
        t = time.time()
        last_updated = self._get_attr_values()
        self.dt_load = self.dt_load * 0.95 + (time.time() - t) * 0.05
        if len(last_updated) > 0:
            self.updateAttrsSignal.emit(last_updated)
            self.updateTimingsSignal.emit(
                {
                    "meas_freq": 1 / self.dt_update,
                    "load": self.dt_load / self.dt_update,
                    "getter_dt": self.timed_getter.dt,
                }
            )
        self.mutx.unlock()

    def _device_appeared(self, device, node_id):
        self.mutx.lock()
        try:
            display_name = "{}{}".format(device.name, node_id)
            self.logger.info("Found {} (uid {})".format(display_name, device.uid))
        except ResponseError as e:
            self.handleErrorSignal.emit(e)
        self.devices_by_name[display_name] = device
        self.names_by_id[node_id] = display_name
        device.name = display_name
        device.include_base_name = True
        self.dynamic_attrs = get_dynamic_attrs(self.devices_by_name)
        self.regenSignal.emit(dict(self.devices_by_name))
        self.mutx.unlock()

    def _device_disappeared(self, node_id):
        self.mutx.lock()
        display_name = "{}{}".format(self.names_by_id[node_id], node_id)
        del self.devices_by_name[display_name]
        del self.names_by_id[node_id]
        self.dynamic_attrs = get_dynamic_attrs(self.devices_by_name)
        self.regenSignal.emit(dict(self.devices_by_name))
        self.mutx.unlock()
