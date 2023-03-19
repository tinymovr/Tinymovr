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
from functools import partial
from contextlib import suppress
import json
from PySide2 import QtCore
from PySide2.QtCore import Signal
from PySide2.QtWidgets import (
    QMainWindow,
    QMenu,
    QMenuBar,
    QAction,
    QWidget,
    QFrame,
    QHBoxLayout,
    QVBoxLayout,
    QHeaderView,
    QLabel,
    QTreeWidget,
    QTreeWidgetItem,
    QPushButton,
)
import pyqtgraph as pg
from tinymovr.constants import app_name
from tinymovr.channel import ResponseError as ChannelResponseError
from tinymovr.config import get_bus_config, configure_logging
from avlos import get_registry
from avlos.json_codec import AvlosEncoder
from tinymovr.gui import (
    Worker,
    format_value,
    load_icon,
    display_file_open_dialog,
    display_file_save_dialog,
    magnitude_of,
    check_selected_items,
)


class MainWindow(QMainWindow):

    TreeItemCheckedSignal = Signal(dict)

    def __init__(self, app, arguments):
        super(MainWindow, self).__init__()

        # set units default format
        get_registry().default_format = ".6f~"

        self.start_time = time.time()
        self.logger = configure_logging()
        bitrate = int(arguments["--bitrate"])

        self.attr_widgets_by_id = {}
        self.graphs_by_id = {}

        self.setWindowTitle(app_name)

        # Create a menu bar, menus and actions
        self.menu_bar = QMenuBar()

        self.file_menu = QMenu("File")

        self.export_action = QAction("Export Config...", self)
        self.import_action = QAction("Import Config", self)
        self.export_action.triggered.connect(self.on_export)
        self.import_action.triggered.connect(self.on_import)
        self.file_menu.addAction(self.export_action)
        self.file_menu.addAction(self.import_action)

        self.menu_bar.addMenu(self.file_menu)
        self.setMenuBar(self.menu_bar)

        # Setup the tree widget
        self.tree_widget = QTreeWidget()
        self.tree_widget.itemChanged.connect(self.item_changed)
        self.tree_widget.itemDoubleClicked.connect(self.double_click)
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
        self.left_frame.setMinimumWidth(340)
        self.left_frame.setMaximumWidth(460)
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

        buses = arguments["--bus"].rsplit(sep=",")
        channel = arguments["--chan"]
        bitrate = int(arguments["--bitrate"])

        if channel == None:
            params = get_bus_config(buses)
            params["bitrate"] = bitrate
        else:
            params = {"bustype": buses[0], "channel": channel, "bitrate": bitrate}

        self.thread = QtCore.QThread()
        self.worker = Worker(params, self.logger)
        self.TreeItemCheckedSignal.connect(self.worker.update_active_attrs)
        self.thread.started.connect(self.worker.run)
        self.worker.moveToThread(self.thread)
        self.worker.handle_error.connect(self.handle_worker_error)
        self.worker.regen.connect(self.regen_tree)
        self.worker.update_attrs.connect(self.attrs_updated)
        app.aboutToQuit.connect(self.about_to_quit)
        self.thread.start()

    @QtCore.Slot()
    def about_to_quit(self):
        self.worker.stop()
        self.thread.quit()
        self.thread.wait()

    @QtCore.Slot()
    def handle_worker_error(self, e):
        if isinstance(e, ChannelResponseError):
            self.logger.warn("Timeout occured")
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
    def regen_tree(self, tms_by_id):
        """
        Regenerate the attribute tree
        """
        self.delete_graphs()
        self.attr_widgets_by_id = {}
        self.tree_widget.clear()
        self.tree_widget.setEnabled(False)
        all_items = []
        for name, node in tms_by_id.items():
            widget, items_list = self.parse_node(node, name)
            self.tree_widget.addTopLevelItem(widget)
            all_items.extend(items_list)
        for item in all_items:
            if hasattr(item, "_tm_function"):
                button = QPushButton("")
                button.setIcon(load_icon("call.png"))
                self.tree_widget.setItemWidget(item, 1, button)
                button.clicked.connect(partial(self.f_call_clicked, item._tm_function))
        header = self.tree_widget.header()
        header.setSectionResizeMode(QHeaderView.ResizeToContents)
        header.setStretchLastSection(False)
        self.tree_widget.setEnabled(True)

    def parse_node(self, node, name):
        widget = QTreeWidgetItem([name, 0, ""])
        widget._orig_flags = widget.flags()
        all_items = []
        if hasattr(node, "remote_attributes"):
            for attr_name, attr in node.remote_attributes.items():
                items, items_list = self.parse_node(attr, attr_name)
                widget.addChild(items)
                all_items.extend(items_list)
        elif hasattr(node, "get_value"):
            widget.setText(1, format_value(node.get_value()))
            widget.setCheckState(0, QtCore.Qt.Unchecked)
            widget._tm_attribute = node
            widget._editing = False
            widget._checked = False
            self.attr_widgets_by_id[node.full_name] = {
                "node": node,
                "widget": widget,
            }
            all_items.append(widget)
        elif hasattr(node, "__call__"):
            widget._tm_function = node
            all_items.append(widget)
        return widget, all_items

    @QtCore.Slot()
    def item_changed(self, item):
        # Value changed
        if item._editing:
            item._editing = False
            attr = item._tm_attribute
            attr.set_value(get_registry()(item.text(1)))
            if "reload_data" in attr.meta and attr.meta["reload_data"]:
                self.worker.reset()
                return
            else:
                item.setText(1, format_value(attr.get_value()))

        # Checkbox changed
        if hasattr(item, "_tm_attribute"):
            attr = item._tm_attribute
            attr_name = attr.full_name
            checked = item.checkState(0) == QtCore.Qt.Checked
            if checked != item._checked:
                item._checked = checked
                self.TreeItemCheckedSignal.emit({"attr": attr, "checked": checked})
                if checked and attr_name not in self.graphs_by_id:
                    self.add_graph_for_attr(attr)
                elif not checked and attr_name in self.graphs_by_id:
                    self.delete_graph_by_attr_name(attr_name)

    @QtCore.Slot()
    def attrs_updated(self, data):
        for attr_name, val in data.items():
            self.attr_widgets_by_id[attr_name]["widget"].setText(1, format_value(val))
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
        self.status_label.setText(
            "{:.1f}Hz\t CH:{:.0f}%\t RT:{:.1f}ms".format(
                1 / self.worker._rate_limited_update.meas_dt,
                self.worker._rate_limited_update.load * 100,
                self.worker.timed_getter.dt * 1000,
            )
        )

    @QtCore.Slot()
    def f_call_clicked(self, f):
        f()
        if "reload_data" in f.meta and f.meta["reload_data"]:
            self.worker.reset()

    @QtCore.Slot()
    def double_click(self, item, column):
        if (
            column == 1
            and hasattr(item, "_tm_attribute")
            and hasattr(item._tm_attribute, "setter_name")
            and item._tm_attribute.setter_name != None
        ):
            item.setFlags(item.flags() | QtCore.Qt.ItemIsEditable)
            item._editing = True
        elif int(item._orig_flags) != int(item.flags()):
            item.setFlags(item._orig_flags)

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
