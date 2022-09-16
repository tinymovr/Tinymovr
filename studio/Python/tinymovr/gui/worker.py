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

    def __init__(self, bustype, channel, bitrate, logger):
        super().__init__()
        self.logger = logger

        init_tee(can.Bus(bustype=bustype, channel=channel, bitrate=bitrate))
        self.dsc = Discovery(self.node_appeared, self.node_disappeared, self.logger)

        self.target_dt = 0.02 # target refresh period
        self.tau = 0.01 # filter value for statistics
        self.meas_dt = self.target_dt # measured (actual) refresh period
        self.rt_dt = 0.0 # round-trip time
        self.load = 0 # channel load, 0-1

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
                self.load = (
                    self.load * (1 - self.tau) + busy_dt / self.target_dt * self.tau
                )
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
        """
        Get new values from the channel, for the selected
        attributes and those marked as dynamic. The values
        are returned as a dictionary with the full attribute
        name as key.
        """
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
            if start_time - t > self.dynamic_attrs_update_period:
                vals[attr.full_name] = self.get_value_meas(attr)
                self.dynamic_attrs_last_update[attr.full_name] = start_time
                break
        return vals

    def get_value_meas(self, attr):
        """
        Get the value of the passed attribute and measure
        the round-trip time for getting it. Update the
        instance statistic.
        """
        get_start_time = time.time()
        val = attr.get_value()
        get_dt = time.time() - get_start_time
        self.rt_dt = self.rt_dt * (1 - self.tau) + get_dt * self.tau
        return val

    def node_appeared(self, node, name):
        """
        Handle the appearance of a new node
        """
        node_name = "{}{}".format(base_node_name, name)
        self.tms_by_id[node_name] = node
        node.name = node_name
        node.include_base_name = True
        self.dynamic_attrs = get_dynamic_attrs(self.tms_by_id)
        self.regen.emit(self.tms_by_id)

    def node_disappeared(self, name):
        """
        Handle the disappearance of an existing node
        """
        node_name = "{}{}".format(base_node_name, name)
        del self.tms_by_id[node_name]
        self.dynamic_attrs = get_dynamic_attrs(self.tms_by_id)
        self.regen.emit(self.tms_by_id)

    @QtCore.Slot(dict)
    def update_active_attrs(self, d):
        """
        Update the active (selected) attributes
        """
        attr = d["attr"]
        if d["enabled"] == True and attr not in self.active_attrs:
            self.active_attrs.append(attr)
        elif d["enabled"] == False and attr in self.active_attrs:
            self.active_attrs.remove(attr)


def get_dynamic_attrs(attr_dict):
    """
    Get the attributes that are marked as dynamic in the
    spec. Returns an array of attribute objects.
    """
    dynamic_attrs = []
    for _, attr in attr_dict.items():
        if isinstance(attr, RemoteAttribute):
            if attr.dynamic_value == True:
                dynamic_attrs.append(attr)
        elif hasattr(attr, "remote_attributes"):
            dynamic_attrs.extend(get_dynamic_attrs(attr.remote_attributes))
    return dynamic_attrs
