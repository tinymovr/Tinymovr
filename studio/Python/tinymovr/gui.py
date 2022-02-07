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
from PySide2.QtWidgets import (QApplication, QMainWindow,
    QWidget, QFrame, QHBoxLayout, QVBoxLayout, QHeaderView,
    QLabel, QTreeWidget, QTreeWidgetItem)
import pyqtgraph as pg
from avlos import get_all
from tinymovr.isotp_channel import LiteBus
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

    TreeItemCheckedSignal = Signal(dict)

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
        bitrate = int(arguments["--bitrate"])

        self.attribute_widgets = []

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

        ############ THREAD
        self.thread = QtCore.QThread()
        self.worker = Worker(bitrate, self.logger)
        self.TreeItemCheckedSignal.connect(self.worker.update_attrs)
        self.thread.started.connect(self.worker.run)
        self.worker.moveToThread(self.thread)
        self.worker.regen.connect(self.regen_tree)
        self.worker.recv.connect(self.update_data)
        self.thread.start()

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

    @QtCore.Slot()
    def regen_tree(self, tms_by_id):
        self.attribute_widgets = []
        self.tree_widget.clear()
        for name, node in tms_by_id.items():
            self.tree_widget.addTopLevelItem(self.parse_node(node, name, self.attribute_widgets))
        header = self.tree_widget.header()
        header.setSectionResizeMode(QHeaderView.ResizeToContents)
        header.setStretchLastSection(False)

    def parse_node(self, node, name, attribute_widgets):
        qt_node = QTreeWidgetItem([name, 0, ""])
        try:
            # Let's assume it's a RemoteObject
            for name, child in node.children.items():
                qt_node.addChild(self.parse_node(child, name, attribute_widgets))
        except AttributeError:
            # Maybe a RemoteAttribute
            try:
                val = node.get_value()
                qt_node.setText(1, format_value(val))
                qt_node.setCheckState(0, QtCore.Qt.Unchecked)
                attribute_widgets.append((node, qt_node))
            except AttributeError:
                # Must be a RemoteFunction then
                qt_node.setText(2, "CALL")
        return qt_node

    @QtCore.Slot()
    def item_changed(self, item):
        enabled = (item.checkState(0) == QtCore.Qt.Checked)
        for attr, widget in self.attribute_widgets:
            if item == widget:
                self.TreeItemCheckedSignal.emit({"attr":attr, "enabled": enabled})
                break

    @QtCore.Slot()
    def update_data(self, data):
        print(data)

    def update_graphs(self):
        pass


class Worker(QObject):

    recv = QtCore.Signal(dict)
    regen = QtCore.Signal(dict)

    def __init__(self, bitrate, logger):
        super().__init__()
        self.logger = logger
        self.bus = LiteBus(bitrate)
        self.dsc = Discovery(self.bus, self.node_appeared, self.node_disappeared, self.logger)
        self.active_attrs = []
        self.tms_by_id = {}

    def run(self):
        while True:
            QApplication.processEvents()
            self.get_values()
            time.sleep(0.01)
            
    def get_values(self):
        for attr in self.active_attrs:
            print(attr.get_value())

    def node_appeared(self, node, node_id):
        node_name = "{}{}".format(base_node_name, node_id)
        self.tms_by_id[node_name] = node
        self.regen.emit(self.tms_by_id)

    def node_disappeared(self, node_id):
        node_name = "{}{}".format(base_node_name, node_id)
        del self.tms_by_id[node_name]
        self.regen.emit(self.tms_by_id)

    @QtCore.Slot( dict )
    def update_attrs(self, d):
        attr = d["attr"]
        if d["enabled"] == True and attr not in self.active_attrs:
            self.active_attrs.append(attr)
        elif d["enabled"] == False and attr in self.active_attrs:
            self.active_attrs.remove(attr)
        print(self.active_attrs)


def format_value(value):
    if isinstance(value, float):
        return "{0:.6g}".format(value)
    return str(value)


def spawn():
    version = pkg_resources.require("tinymovr")[0].version
    arguments = docopt(__doc__, version=app_name + " " + str(version))
    app = QApplication(sys.argv)
    w = MainWindow(arguments)
    w.show()
    app.exec_()
