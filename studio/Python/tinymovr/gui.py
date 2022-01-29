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
from docopt import docopt

import can
from tinymovr.isotp_channel import guess_channel
from tinymovr.discovery import Discovery
from tinymovr.constants import app_name, base_node_name
from tinymovr.config import configure_logging
from PySide2 import QtCore
from PySide2.QtWidgets import QApplication, QMainWindow, QWidget, QFrame, QHBoxLayout, QVBoxLayout, QPushButton, QComboBox, QTreeWidget, QTreeWidgetItem
import pyqtgraph as pg

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

        logger = configure_logging()
        bustype = arguments["--bustype"]
        channel = arguments["--chan"]
        bitrate = int(arguments["--bitrate"])
        if channel == "auto":
            channel = guess_channel(bustype, logger)
        can_bus = can.Bus(bustype=bustype, channel=channel, bitrate=bitrate)
        
        self.dsc = Discovery(can_bus, self.node_appeared, self.node_disappeared, logger)
        self.nodes = {}
        self.attribute_widgets = []

        self.setWindowTitle(app_name)
        self.tree_widget = QTreeWidget()
        headers = ["Attribute", "Value"]
        self.tree_widget.setHeaderLabels(headers)
        self.make_graph()

        left_frame = QFrame(self)
        left_layout = QVBoxLayout()
        left_frame.setLayout(left_layout)
        left_layout.addWidget(self.tree_widget)

        main_layout = QHBoxLayout()
        main_layout.addWidget(left_frame)
        main_layout.addWidget(self.graph_widget)

        main_widget = QWidget()
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)

        self.update_thread = threading.Thread(target=self.update_thread, daemon=True)
        self.update_thread.start()

    def make_graph(self):
        self.graph_widget = pg.PlotWidget()

        hour = [1,2,3,4,5,6,7,8,9,10]
        temperature = [30,32,34,32,33,31,29,32,35,45]

        # plot data: x, y values
        self.graph_widget.plot(hour, temperature)

    def node_appeared(self, node, node_id):
        node_name = "{}{}".format(base_node_name, node_id)
        self.nodes[node_name] = node
        self.regen_tree()

    def node_disappeared(self, node_id):
        node_name = "{}{}".format(base_node_name, node_id)
        del self.nodes[node_name]
        self.regen_tree()

    def regen_tree(self):
        self.attribute_widgets = []
        self.tree_widget.clear()
        for name, node in self.nodes.items():
            self.tree_widget.addTopLevelItem(parse_node(node, name, self.attribute_widgets))

    def update_thread(self):
        while True:
            self.update_values()
            time.sleep(0.02)

    def update_values(self):
        for node, qt_node in self.attribute_widgets:
            if qt_node.checkState(0) == QtCore.Qt.Checked:
                val = node.get_value()
                qt_node.setText(1, str(val))


def parse_node(node, name, attribute_widgets):
    qt_node = QTreeWidgetItem([name, 0])
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
            qt_node.setText(1, "CALL")
    return qt_node


def spawn():
    version = pkg_resources.require("tinymovr")[0].version
    arguments = docopt(__doc__, version=app_name + " " + str(version))
    app = QApplication(sys.argv)
    w = MainWindow(arguments)
    w.show()
    app.exec_()