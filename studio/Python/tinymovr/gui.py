"""Tinymovr Studio

Usage:
    tinymovr [--bitrate=<bitrate>]
    tinymovr -h | --help
    tinymovr --version

Options:
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
"""

import sys
import time
import threading
import pkg_resources
import struct
from docopt import docopt
from queue import Queue
from PySide2 import QtCore
from PySide2.QtCore import QObject, QThread, Signal, Slot
from PySide2.QtWidgets import (
    QApplication,
    QMainWindow,
    QWidget,
    QFrame,
    QHBoxLayout,
    QVBoxLayout,
    QHeaderView,
    QLabel,
    QTreeWidget,
    QTreeWidgetItem,
)
import pyqtgraph as pg
from avlos import get_all
from tinymovr.isotp_channel import LiteBus
from tinymovr.discovery import Discovery
from tinymovr.constants import app_name, base_node_name
from tinymovr.config import configure_logging
from tinymovr.constants import app_name


"""
Copyright 2022 Ioannis Chatzikonstantinou (Tinymovr)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
"""


class MainWindow(QMainWindow):

    TreeItemCheckedSignal = Signal(dict)

    def __init__(self, app, arguments):
        super(MainWindow, self).__init__()

        self.timings = {
            "set_period": 0.01,
            "total_meas_period": {"last": 1, "ravg": 1},
            "can_meas_period": {"last": 1, "ravg": 1},
        }
        self.tau = 0.02

        self.start_time = time.time()
        self.logger = configure_logging()
        bitrate = int(arguments["--bitrate"])

        self.attribute_widgets_by_id = {}

        self.setWindowTitle(app_name)
        self.tree_widget = QTreeWidget()
        self.tree_widget.itemChanged.connect(self.item_changed)
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
        self.graphs_by_id = {}

        self.thread = QtCore.QThread()
        self.worker = Worker(bitrate, self.logger)
        self.TreeItemCheckedSignal.connect(self.worker.update_checkstates)
        self.thread.started.connect(self.worker.run)
        self.worker.moveToThread(self.thread)
        self.worker.regen.connect(self.regen_tree)
        self.worker.update_attrs.connect(self.update_attrs)
        app.aboutToQuit.connect(self.about_to_quit)
        self.thread.start()

    @QtCore.Slot()
    def about_to_quit(self):
        self.worker.stop()
        self.thread.quit()
        self.thread.wait()

    def get_rel_time(self):
        return time.time() - self.start_time

    def make_graph(self, attr):
        graph_widget = pg.PlotWidget(title=attr.name)
        x = []
        y = []
        data_line = graph_widget.plot(x, y)
        return {
            "widget": graph_widget,
            "data": {"x": x, "y": y},
            "data_line": data_line,
        }

    @QtCore.Slot()
    def regen_tree(self, tms_by_id):
        self.attribute_widgets_by_id = {}
        self.tree_widget.clear()
        for name, node in tms_by_id.items():
            self.tree_widget.addTopLevelItem(
                self.parse_node(node, name, self.attribute_widgets_by_id)
            )
        header = self.tree_widget.header()
        header.setSectionResizeMode(QHeaderView.ResizeToContents)
        header.setStretchLastSection(False)

    def parse_node(self, node, name, attribute_widgets_by_id):
        widget = QTreeWidgetItem([name, 0, ""])
        try:
            # Let's assume it's a RemoteObject
            for child_name, child in node.children.items():
                widget.addChild(
                    self.parse_node(child, child_name, attribute_widgets_by_id)
                )
        except AttributeError:
            # Maybe a RemoteAttribute
            try:
                val = node.get_value()
                widget.setText(1, format_value(val))
                widget.setCheckState(0, QtCore.Qt.Unchecked)
                attribute_widgets_by_id[node.id] = {"node": node, "widget": widget}
            except AttributeError:
                # Must be a RemoteFunction then
                widget.setText(2, "CALL")
        return widget

    @QtCore.Slot()
    def item_changed(self, item):
        enabled = item.checkState(0) == QtCore.Qt.Checked
        for attr_id, data in self.attribute_widgets_by_id.items():
            if item == data["widget"]:
                attr = data["node"]
                self.TreeItemCheckedSignal.emit({"attr": attr, "enabled": enabled})
                if enabled and attr_id not in self.graphs_by_id:
                    graph = self.make_graph(attr)
                    self.graphs_by_id[attr_id] = graph
                    self.right_layout.addWidget(graph["widget"])
                elif not enabled and attr_id in self.graphs_by_id:
                    self.graphs_by_id[attr_id]["widget"].deleteLater()
                    del self.graphs_by_id[attr_id]
                break

    @QtCore.Slot()
    def update_attrs(self, data):
        for attr_id, val in data.items():
            self.attribute_widgets_by_id[attr_id]["widget"].setText(
                1, format_value(val)
            )
            if attr_id in self.graphs_by_id:
                graph_info = self.graphs_by_id[attr_id]
                data_line = graph_info["data_line"]
                x = graph_info["data"]["x"]
                y = graph_info["data"]["y"]
                if len(x) >= 200:
                    x.pop(0)
                    y.pop(0)
                x.append(self.get_rel_time())
                y.append(val)
                data_line.setData(x, y)


class Worker(QObject):

    update_attrs = QtCore.Signal(dict)
    regen = QtCore.Signal(dict)

    def __init__(self, bitrate, logger):
        super().__init__()
        self.logger = logger
        self.bus = LiteBus(bitrate)
        self.dsc = Discovery(
            self.bus, self.node_appeared, self.node_disappeared, self.logger
        )
        self.active_attrs = []
        self.tms_by_id = {}
        self.running = True

    def run(self):
        while self.running:
            self.get_values()
            QApplication.processEvents()
            time.sleep(0.01)

    @QtCore.Slot()
    def stop(self):
        self.running = False

    def get_values(self):
        updated_attrs = {attr.id: attr.get_value() for attr in self.active_attrs}
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


def format_value(value):
    if isinstance(value, float):
        return "{0:.6g}".format(value)
    return str(value)


def spawn():
    version = pkg_resources.require("tinymovr")[0].version
    arguments = docopt(__doc__, version=app_name + " " + str(version))
    app = QApplication(sys.argv)
    w = MainWindow(app, arguments)
    w.show()
    sys.exit(app.exec_())