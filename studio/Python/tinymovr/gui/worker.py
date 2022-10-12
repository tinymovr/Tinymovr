import time
from tracemalloc import start
import can
from PySide2 import QtCore
from PySide2.QtCore import QObject
from PySide2.QtWidgets import (
    QApplication,
)
from avlos.definitions import RemoteAttribute
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

        init_tee(can.Bus(**busparams))
        self.dsc = Discovery(self.node_appeared, self.node_disappeared, self.logger)
        self.target_dt = 0.040
        self.meas_dt = self.target_dt
        self.rt_dt = 0.0
        self.load = 0
        self.active_attrs = []
        self.dynamic_attrs = []
        self.tms_by_id = {}
        self.dynamic_attrs_last_update = {}
        self.dynamic_attrs_update_period = 0.5  # sec
        self.running = True

    def run(self):
        while self.running:
            start_time = time.time()
            last_updated = self.get_attr_values()
            if len(last_updated) > 0:
                self.update_attrs.emit(last_updated)
            QApplication.processEvents()
            busy_dt = time.time() - start_time
            if busy_dt < self.target_dt:
                self.load = self.load * 0.99 + busy_dt / self.target_dt * 0.01
                time.sleep(self.target_dt - busy_dt)
                self.meas_dt = self.target_dt
            else:
                self.load = 1
                self.meas_dt = busy_dt
        destroy_tee()

    @QtCore.Slot()
    def stop(self):
        self.running = False

    def get_attr_values(self):
        """ """
        # TODO: Handle possible exception
        vals = {}
        for attr in self.active_attrs:
            vals[attr.full_name] = self.get_value_meas(attr)
        start_time = time.time()
        for attr in self.dynamic_attrs:
            try:
                t = self.dynamic_attrs_last_update[attr.full_name]
            except KeyError:
                t = 0
            if (attr.full_name not in vals) and (
                start_time - t > self.dynamic_attrs_update_period
            ):
                vals[attr.full_name] = self.get_value_meas(attr)
                self.dynamic_attrs_last_update[attr.full_name] = start_time
                break
        return vals

    def get_value_meas(self, attr):
        get_start_time = time.time()
        try:
            val = attr.get_value()
            get_dt = time.time() - get_start_time
            self.rt_dt = self.rt_dt * 0.99 + get_dt * 0.01
            return val
        except Exception as e:
            self.handle_error.emit(e)

    def node_appeared(self, node, name):
        node_name = "{}{}".format(base_node_name, name)
        self.tms_by_id[node_name] = node
        node.name = node_name
        node.include_base_name = True
        self.dynamic_attrs = self.get_dynamic_attrs(self.tms_by_id)
        self.regen.emit(self.tms_by_id)

    def node_disappeared(self, name):
        node_name = "{}{}".format(base_node_name, name)
        del self.tms_by_id[node_name]
        self.dynamic_attrs = self.get_dynamic_attrs(self.tms_by_id)
        self.regen.emit(self.tms_by_id)

    @QtCore.Slot(dict)
    def update_active_attrs(self, d):
        attr = d["attr"]
        if d["enabled"] == True and attr not in self.active_attrs:
            self.active_attrs.append(attr)
        elif d["enabled"] == False and attr in self.active_attrs:
            self.active_attrs.remove(attr)

    def get_dynamic_attrs(self, attr_dict):
        """
        Get the attributes that are marked as dynamic in the spec.
        """
        dynamic_attrs = []
        for _, attr in attr_dict.items():
            if isinstance(attr, RemoteAttribute):
                if attr.dynamic_value == True:
                    dynamic_attrs.append(attr)
            elif hasattr(attr, "remote_attributes"):
                dynamic_attrs.extend(self.get_dynamic_attrs(attr.remote_attributes))
        return dynamic_attrs
