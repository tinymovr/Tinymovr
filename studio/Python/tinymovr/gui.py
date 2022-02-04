"""Tinymovr Studio

Usage:
    tinymovr [--bustype=<bustype>] [--chan=<chan>] [--bitrate=<bitrate>]
    tinymovr -h | --help
    tinymovr --version

Options:
    --bustype=<bustype>  CAN bus type to use [default: slcan].
    --chan=<chan>        CAN channel (i.e. device) to use [default: auto].
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
"""

import sys
import time
import threading
import pkg_resources
import struct
from docopt import docopt
import can
from queue import Queue
from PySide2 import QtCore
from PySide2.QtWidgets import (QApplication, QMainWindow,
    QWidget, QFrame, QHBoxLayout, QVBoxLayout, QHeaderView,
    QLabel, QTreeWidget, QTreeWidgetItem)
import pyqtgraph as pg
from avlos import get_all
from tinymovr.isotp_channel import guess_channel
from tinymovr.discovery import Discovery
from tinymovr.constants import app_name, base_node_name
from tinymovr.config import configure_logging
from tinymovr.constants import app_name


"""
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

class MainWindow(QMainWindow):

    def __init__(self, arguments):
        super(MainWindow, self).__init__()
        
        self.timings = {
            "set_period": 0.01,
            "total_meas_period": {"last":1, "ravg":1},
            "can_meas_period": {"last":1, "ravg":1}
        }
        self.tau=0.02

        self.start_time = time.time()
        self.logger = configure_logging()
        bustype = arguments["--bustype"]
        channel = arguments["--chan"]
        bitrate = int(arguments["--bitrate"])
        if channel == "auto":
            channel = guess_channel(bustype, self.logger)
        self.can_bus = can.Bus(bustype=bustype, channel=channel, bitrate=bitrate)
        
        self.dsc = Discovery(self.can_bus, self.node_appeared, self.node_disappeared, self.logger)
        self.tms_by_id = {}
        self.attribute_widgets = []
        self.last_values = {}
        self.graphs_and_data_by_attr_id = {}

        self.setWindowTitle(app_name)
        self.tree_widget = QTreeWidget()
        headers = ["Attribute", "Value", "Set Value"]
        self.tree_widget.setHeaderLabels(headers)

        self.status_label = QLabel()
        self.status_label.setStyleSheet("margin: 5px;")

        self.left_frame = QFrame(self)
        self.left_layout = QVBoxLayout()
        self.left_layout.addWidget(self.tree_widget)
        self.left_layout.addWidget(self.status_label)
        self.left_layout.setSpacing(0)
        self.left_layout.setContentsMargins(0, 0, 0, 0)
        self.left_frame.setLayout(self.left_layout)
        self.left_frame.setMinimumWidth(340)
        self.left_frame.setMaximumWidth(460)
        self.left_frame.setStyleSheet("border:0;")

        self.right_frame = QFrame(self)
        self.right_layout = QVBoxLayout()
        self.right_layout.setSpacing(0)
        self.right_layout.setContentsMargins(0, 0, 0, 0)
        self.right_frame.setLayout(self.right_layout)
        self.right_frame.setMinimumWidth(820)

        main_layout = QHBoxLayout()
        main_layout.addWidget(self.left_frame)
        main_layout.addWidget(self.right_frame)
        main_layout.setSpacing(0)
        main_layout.setContentsMargins(0, 0, 0, 0)

        main_widget = QWidget()
        main_widget.setLayout(main_layout)
        main_widget.setMinimumHeight(600)
        self.setCentralWidget(main_widget)

        pg.setConfigOptions(antialias=True)

        self.update_thread = threading.Thread(target=self.update_thread, daemon=True)
        self.update_thread.start()

    def get_rel_time(self):
        return time.time() - self.start_time

    def make_graph(self, attr):
        graph_widget = pg.PlotWidget(title=attr.name)
        x = [self.get_rel_time()]
        y = [attr.get_value()]
        data_line =  graph_widget.plot(x, y)
        return ({
            "graph_widget": graph_widget,
            "graph_data": {"x": x, "y": y},
            "data_line": data_line
        })

    def node_appeared(self, node, node_id):
        node_name = "{}{}".format(base_node_name, node_id)
        self.tms_by_id[node_name] = node
        self.regen_tree()

    def node_disappeared(self, node_id):
        node_name = "{}{}".format(base_node_name, node_id)
        del self.tms_by_id[node_name]
        self.regen_tree()

    def regen_tree(self):
        self.attribute_widgets = []
        self.tree_widget.clear()
        for name, node in self.tms_by_id.items():
            self.tree_widget.addTopLevelItem(parse_node(node, name, self.attribute_widgets))
        header = self.tree_widget.header()
        header.setSectionResizeMode(QHeaderView.ResizeToContents)
        header.setStretchLastSection(False)

    def update_thread(self):
        while True:
            start_all = time.time()
            QApplication.instance().processEvents()
            start_can = time.time()
            self.get_values()
            delta_can = time.time() - start_can
            invoke_in_main_thread(self.update_graphs)
            delta_all = time.time() - start_all
            self.timings["total_meas_period"]["last"] = delta_all
            self.timings["total_meas_period"]["ravg"] = self.timings["total_meas_period"]["ravg"] * (1-self.tau) + delta_all * self.tau
            self.timings["can_meas_period"]["last"] = delta_can
            self.timings["can_meas_period"]["ravg"] = self.timings["total_meas_period"]["ravg"] * (1-self.tau) + delta_can * self.tau
            try:
                hz = min(1.0/self.timings["set_period"], 1.0/self.timings["total_meas_period"]["ravg"])
            except ZeroDivisionError:
                pass
            self.status_label.setText(
                "can: {0:.3f} s\t total: {1:.3f} s\t {2:.1f} Hz".format(
                    self.timings["can_meas_period"]["ravg"],
                    self.timings["total_meas_period"]["ravg"],
                    hz))
            if delta_all < self.timings["set_period"]:
                time.sleep(self.timings["set_period"] - delta_all)

    def get_values(self):
        attrs = [attr for attr, qt_node in self.attribute_widgets if qt_node.checkState(0) == QtCore.Qt.Checked]
        if len(attrs) > 0:
            try:
                vals = get_all(attrs, attrs[0].root.serializer)
                for attr, val in zip(attrs, vals):
                    self.last_values[attr.id] = val
            except struct.error as e:
                self.logger.warn("Error while getting multiple values: {}".format(str(e)))
    
    def update_graphs(self):
        for attr, qt_node in self.attribute_widgets:
            if qt_node.checkState(0) == QtCore.Qt.Checked:
                try:
                    val = self.last_values[attr.id]
                    qt_node.setText(1, str(val))
                    if attr.id not in self.graphs_and_data_by_attr_id:
                        print("Added")
                        graph_info = self.make_graph(attr)
                        self.graphs_and_data_by_attr_id[attr.id] = graph_info
                        self.right_layout.addWidget(graph_info["graph_widget"])
                except KeyError:
                    pass
            else:
                if attr.id in self.graphs_and_data_by_attr_id:
                    print("Removed")
                    graph_info = self.graphs_and_data_by_attr_id[attr.id]
                    graph_info["graph_widget"].deleteLater()
                    del self.graphs_and_data_by_attr_id[attr.id]
        for attr_id, graph_info in self.graphs_and_data_by_attr_id.items():
            data_line = graph_info["data_line"]
            x = graph_info["graph_data"]["x"]
            y = graph_info["graph_data"]["y"]
            if (len(x) >= 200):
                x.pop(0)
                y.pop(0)
            x.append(self.get_rel_time())
            y.append(self.last_values[attr_id])
            data_line.setData(x, y)


def parse_node(node, name, attribute_widgets):
    qt_node = QTreeWidgetItem([name, 0, ""])
    try:
        # Let's assume it's a RemoteObject
        for name, child in node.children.items():
            qt_node.addChild(parse_node(child, name, attribute_widgets))
    except AttributeError:
        # Maybe a RemoteAttribute
        try:
            val = node.get_value()
            qt_node.setText(1, str(val))
            qt_node.setCheckState(0, QtCore.Qt.Unchecked)
            attribute_widgets.append((node, qt_node))
        except AttributeError:
            # Must be a RemoteFunction then
            qt_node.setText(2, "CALL")
    return qt_node


def spawn():
    version = pkg_resources.require("tinymovr")[0].version
    arguments = docopt(__doc__, version=app_name + " " + str(version))
    app = QApplication(sys.argv)
    w = MainWindow(arguments)
    w.show()
    app.exec_()


class Invoker(QtCore.QObject):
    def __init__(self):
        super(Invoker, self).__init__()
        self.queue = Queue()

    def invoke(self, func, *args):
        f = lambda: func(*args)
        self.queue.put(f)
        QtCore.QMetaObject.invokeMethod(self, "handler", QtCore.Qt.QueuedConnection)

    @QtCore.Slot()
    def handler(self):
        f = self.queue.get()
        f()
invoker = Invoker()

def invoke_in_main_thread(func, *args):
    invoker.invoke(func,*args)