"""
Tinymovr GUI Window
Copyright Ioannis Chatzikonstantinou 2020-2023

The GUI Window class that subclasses QMainWindow

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
import logging
import pkg_resources
from contextlib import suppress
import json
from PySide6 import QtCore
from PySide6.QtCore import Signal, QTimer
from PySide6.QtWidgets import (
    QMainWindow,
    QMenu,
    QMenuBar,
    QWidget,
    QFrame,
    QHBoxLayout,
    QVBoxLayout,
    QHeaderView,
    QLabel,
    QMessageBox,
    QTreeWidgetItem,
)
from PySide6.QtGui import QAction
import pyqtgraph as pg
from tinymovr.constants import app_name
from tinymovr.channel import ResponseError as ChannelResponseError
from tinymovr.config import get_bus_config
from avlos import get_registry
from avlos.datatypes import DataType
from avlos.json_codec import AvlosEncoder
from tinymovr.gui import (
    NodeTreeWidgetItem,
    AttrTreeWidgetItem,
    FuncTreeWidgetItem,
    OptionsTreeWidgetItem,
    Worker,
    PlaceholderQTreeWidget,
    BoolTreeWidgetItem,
    format_value,
    display_file_open_dialog,
    display_file_save_dialog,
    magnitude_of,
    check_selected_items,
)


class MainWindow(QMainWindow):
    TreeItemCheckedSignal = Signal(dict)
    updateVisibleAttrsSignal = Signal(set)

    def __init__(self, app, arguments, logger):
        super(MainWindow, self).__init__()

        # set units default format
        get_registry().default_format = ".6f~"

        self.start_time = time.time()
        if logger is None:
            self.logger = logging.getLogger("tinymovr")
        else:
            self.logger = logger

        self.attr_widgets_by_id = {}
        self.graphs_by_id = {}

        self.setWindowTitle(app_name)

        # Create a menu bar, menus and actions
        self.menu_bar = QMenuBar()

        self.file_menu = QMenu("File")
        self.help_menu = QMenu("Help")

        self.export_action = QAction("Export Config...", self)
        self.import_action = QAction("Import Config", self)
        self.about_action = QAction("About", self)
        self.export_action.triggered.connect(self.on_export)
        self.import_action.triggered.connect(self.on_import)
        self.about_action.triggered.connect(self.show_about_box)
        self.file_menu.addAction(self.export_action)
        self.file_menu.addAction(self.import_action)
        self.help_menu.addAction(self.about_action)

        self.menu_bar.addMenu(self.file_menu)
        self.menu_bar.addMenu(self.help_menu)
        self.setMenuBar(self.menu_bar)

        # Setup the tree widget
        self.tree_widget = PlaceholderQTreeWidget()
        self.tree_widget.itemChanged.connect(self.item_changed)
        self.tree_widget.itemExpanded.connect(self.update_visible_attrs)
        self.tree_widget.itemCollapsed.connect(self.update_visible_attrs)
        self.tree_widget.setHeaderLabels(["Attribute", "Value"])

        self.status_label = QLabel()
        self.status_label.setStyleSheet("margin: 5px;")

        self.left_frame = QFrame(self)
        self.left_layout = QVBoxLayout()
        self.left_layout.addWidget(self.tree_widget)
        self.left_layout.addWidget(self.status_label)
        self.left_layout.setSpacing(0)
        self.left_layout.setContentsMargins(0, 0, 0, 0)
        self.left_frame.setLayout(self.left_layout)
        self.left_frame.setMinimumWidth(320)
        self.left_frame.setMaximumWidth(420)
        self.left_frame.setStyleSheet("border:0;")

        self.right_frame = QFrame(self)
        self.right_layout = QVBoxLayout()
        self.right_layout.setSpacing(0)
        self.right_layout.setContentsMargins(0, 0, 0, 0)
        self.right_frame.setLayout(self.right_layout)

        main_layout = QHBoxLayout()
        main_layout.addWidget(self.left_frame)
        main_layout.addWidget(self.right_frame)
        main_layout.setSpacing(0)
        main_layout.setContentsMargins(0, 0, 0, 0)

        main_widget = QWidget()
        main_widget.setLayout(main_layout)
        main_widget.setMinimumHeight(600)
        self.setCentralWidget(main_widget)

        self.timeout_count = 0

        buses = arguments["--bus"].rsplit(sep=",")
        channel = arguments["--chan"]
        bitrate = int(arguments["--bitrate"])
        self.max_timeouts = int(arguments["--max-timeouts"])

        if channel == None:
            params = get_bus_config(buses)
            params["bitrate"] = bitrate
        else:
            params = {"bustype": buses[0], "channel": channel, "bitrate": bitrate}

        self.worker = Worker(params, self.logger)

        self.TreeItemCheckedSignal.connect(self.worker.update_active_attrs)
        self.worker.handleErrorSignal.connect(
            self.handle_worker_error, QtCore.Qt.QueuedConnection
        )
        self.worker.regenSignal.connect(self.regen_tree, QtCore.Qt.QueuedConnection)
        self.worker.updateAttrsSignal.connect(
            self.attrs_updated, QtCore.Qt.QueuedConnection
        )
        self.worker.updateTimingsSignal.connect(
            self.timings_updated, QtCore.Qt.QueuedConnection
        )
        self.updateVisibleAttrsSignal.connect(self.worker.update_visible_attrs)
        app.aboutToQuit.connect(self.about_to_quit)

        self.worker_update_timer = QTimer(self)
        self.worker_update_timer.timeout.connect(self.worker._update)
        self.worker_update_timer.start(40)

        self.visibility_update_timer = QTimer(self)
        self.visibility_update_timer.timeout.connect(self.update_visible_attrs)
        self.visibility_update_timer.start(1000)

    @QtCore.Slot()
    def about_to_quit(self):
        self.visibility_update_timer.stop()
        self.worker_update_timer.stop()
        self.worker.stop()

    @QtCore.Slot()
    def handle_worker_error(self, e):
        if isinstance(e, ChannelResponseError):
            if self.timeout_count > self.max_timeouts:
                self.timeout_count = 0
                self.logger.warn("Max timeouts exceeded. Triggering rescan...")
                self.worker.reset()
            else:
                self.logger.warn("Timeout while getting value.")
                self.timeout_count += 1

        else:
            raise e

    def add_graph_for_attr(self, attr):
        """
        Create a new pyqtgraph object, for the passed
        attribute. Add a dictionary containing the
        object and related data container to the local index.
        """
        graph_widget = pg.PlotWidget(title=attr.full_name)
        graph_widget.setMinimumWidth(800)
        pi = graph_widget.getPlotItem()
        pi.skipFiniteCheck = True
        if attr.unit:
            pi.setLabel(axis="left", text=attr.name, units=f"{attr.unit}")
        else:
            pi.setLabel(axis="left", text=attr.name)
        pi.setLabel(axis="bottom", text="time", units="sec")
        data = {"x": [], "y": []}
        data_line = pg.PlotCurveItem(data["x"], data["y"], pen=pg.mkPen(width=1.00))
        graph_widget.addItem(data_line)
        self.graphs_by_id[attr.full_name] = {
            "widget": graph_widget,
            "data": data,
            "data_line": data_line,
        }
        self.right_layout.addWidget(graph_widget)

    @QtCore.Slot()
    def update_visible_attrs(self):
        """
        Collects names of visible attributes and emits a signal with these names.
        """
        visible_attrs = {
            attr_name
            for attr_name, widget_info in self.attr_widgets_by_id.items()
            if self.is_widget_visible(widget_info["widget"])
            and self.is_item_visible_in_viewport(widget_info["widget"])
        }
        self.updateVisibleAttrsSignal.emit(visible_attrs)

    @QtCore.Slot()
    def regen_tree(self, devices_by_name):
        """
        Regenerate the attribute tree
        """
        self.delete_graphs()
        self.attr_widgets_by_id = {}
        self.tree_widget.clear()
        self.tree_widget.setEnabled(False)
        for name, device in devices_by_name.items():
            widget = self.parse_node(device, name)
            widget.add_to_tree(self.tree_widget)
        header = self.tree_widget.header()
        header.setSectionResizeMode(QHeaderView.ResizeToContents)
        header.setStretchLastSection(True)
        self.tree_widget.setEnabled(True)

    def parse_node(self, node, name):
        if hasattr(node, "remote_attributes"):
            widget = NodeTreeWidgetItem(name)
            for attr_name, attr in node.remote_attributes.items():
                attr_widgets_node_widget = self.parse_node(attr, attr_name)
                widget.addChild(attr_widgets_node_widget)
        elif hasattr(node, "__call__"):
            widget = FuncTreeWidgetItem(name, node)
        else:
            if hasattr(node, "options"):
                widget = OptionsTreeWidgetItem(name, node)
            elif hasattr(node, "get_value"):
                if node.dtype == DataType.BOOL:
                    widget = BoolTreeWidgetItem(name, node)
                else:
                    widget = AttrTreeWidgetItem(name, node)
            self.attr_widgets_by_id[node.full_name] = {
                "node": node,
                "widget": widget,
            }
        return widget

    @QtCore.Slot()
    def item_changed(self, item):
        if item._on_checkbox_changed():
            attr = item._tm_node
            attr_name = attr.full_name
            checked = item._checked
            self.TreeItemCheckedSignal.emit({"attr": attr, "checked": checked})
            if checked and attr_name not in self.graphs_by_id:
                self.add_graph_for_attr(attr)
            elif not checked and attr_name in self.graphs_by_id:
                self.delete_graph_by_attr_name(attr_name)

    @QtCore.Slot()
    def attrs_updated(self, data):
        for attr_name, val in data.items():
            try:
                self.attr_widgets_by_id[attr_name]["widget"].set_text(format_value(val))
            except RuntimeError:
                self.logger.warn("Attribute widget disappeared while updating")
            if attr_name in self.graphs_by_id:
                graph_info = self.graphs_by_id[attr_name]
                x = graph_info["data"]["x"]
                y = graph_info["data"]["y"]
                if len(x) >= 200:
                    x.pop(0)
                    y.pop(0)
                x.append(time.time() - self.start_time)
                y.append(magnitude_of(val))
                graph_info["data_line"].setData(x, y)
                graph_info["widget"].update()

    @QtCore.Slot()
    def timings_updated(self, timings_dict):
        meas_freq = timings_dict["meas_freq"]
        meas_freq_str = "-" if meas_freq == 0 else "{:.1f}Hz".format(meas_freq)
        self.status_label.setText(
            "{}\t Load:{:.0f}%\t RT:{:.1f}ms".format(
                meas_freq_str,
                timings_dict["load"] * 100,
                timings_dict["getter_dt"] * 1000,
            )
        )

    def on_export(self):
        selected_items = self.tree_widget.selectedItems()
        if check_selected_items(selected_items):
            root_node = selected_items[0]._tm_attribute.root
            values_object = root_node.export_values()
            json_data = json.dumps(values_object, cls=AvlosEncoder)
            file_path = display_file_save_dialog()
            with suppress(FileNotFoundError), open(file_path, "w") as file:
                file.write(json_data)

    def on_import(self):
        selected_items = self.tree_widget.selectedItems()
        if check_selected_items(selected_items):
            root_node = selected_items[0]._tm_attribute.root
            file_path = display_file_open_dialog()
            with suppress(FileNotFoundError), open(file_path, "r") as file:
                values_object = json.load(file)
                root_node.import_values(values_object)
                time.sleep(0.1)
                self.worker.force_regen()

    def delete_graph_by_attr_name(self, attr_name):
        self.graphs_by_id[attr_name]["widget"].deleteLater()
        del self.graphs_by_id[attr_name]

    def delete_graphs(self):
        for attr_name in list(self.graphs_by_id.keys()):
            self.delete_graph_by_attr_name(attr_name)

    def show_about_box(self):
        version_str = pkg_resources.require("tinymovr")[0].version
        app_str = "{} {}".format(app_name, version_str)
        QMessageBox.about(
            self,
            "About Tinymovr",
            "{}\nhttps://tinymovr.com\n\nCat Sleeping Icon by Denis Sazhin from Noun Project".format(
                app_str
            ),
        )

    def is_widget_visible(self, widget):
        """
        Check if the given widget is visible, i.e., not hidden and all its
        ancestor widgets are expanded.
        """
        if widget.isHidden():
            return False
        parent = widget.parent()
        while parent is not None:
            if isinstance(parent, QTreeWidgetItem) and not parent.isExpanded():
                return False
            parent = parent.parent()
        return True

    def is_item_visible_in_viewport(self, item):
        """
        Check if the QTreeWidgetItem is visible in the viewport of the QTreeWidget.
        """
        # Get the item's rectangle in tree widget coordinates
        rect = self.tree_widget.visualItemRect(item)

        # Check if the rectangle is within the visible region of the tree widget
        visible_region = self.tree_widget.visibleRegion()
        return visible_region.contains(rect)
