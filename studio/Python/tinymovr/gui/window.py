import time
from PySide2 import QtCore
from PySide2.QtCore import Signal
from PySide2.QtWidgets import (
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
from tinymovr.constants import app_name
from tinymovr.config import get_bus_config, configure_logging
from tinymovr.gui import Worker, format_value, load_icon

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
        self.tree_widget.itemDoubleClicked.connect(self.double_click)
        headers = ["Attribute", "Value"]
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

        buses = arguments["--bus"].rsplit(sep=",")
        channel = arguments["--chan"]
        if not channel:
            bustype, channel = get_bus_config(buses)
        else:
            bustype = buses[0]

        self.thread = QtCore.QThread()
        self.worker = Worker(bustype, channel, bitrate, self.logger)
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
        graph_widget = pg.PlotWidget(title=attr.full_name)
        pi = graph_widget.getPlotItem()
        if attr.unit:
            pi.setLabel(axis="left", text=attr.name, units=f"{attr.unit}")
        else:
            pi.setLabel(axis="left", text=attr.name)
        pi.setLabel(axis="bottom", text="time", units="sec")
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
        widget._orig_flags = widget.flags()
        try:
            # Let's assume it's a RemoteNode
            for attr_name, attr in node.remote_attributes.items():
                widget.addChild(
                    self.parse_node(attr, attr_name, attribute_widgets_by_id)
                )
        except AttributeError:
            # Maybe a RemoteAttribute
            try:
                val = node.get_value()
                widget.setText(1, format_value(val))
                widget.setCheckState(0, QtCore.Qt.Unchecked)
                widget._tm_attribute = node
                widget._editing = False
                attribute_widgets_by_id[node.ep_id] = {"node": node, "widget": widget}
            except AttributeError:
                # Must be a RemoteFunction then
                widget._tm_function = node
                widget.setIcon(1, load_icon("call.png"))
        return widget

    @QtCore.Slot()
    def item_changed(self, item):
        if item._editing:
            # Value changed
            val = item.text(1)
            item._tm_attribute.set_value(float(val))
            item.setText(1, format_value(item._tm_attribute.get_value()))

        item._editing = False
        # Checkbox changed
        enabled = item.checkState(0) == QtCore.Qt.Checked
        try:
            attr = item._tm_attribute
        except AttributeError:
            return
        attr_id = attr.ep_id
        self.TreeItemCheckedSignal.emit({"attr": attr, "enabled": enabled})
        if enabled and attr_id not in self.graphs_by_id:
            graph = self.make_graph(attr)
            self.graphs_by_id[attr_id] = graph
            self.right_layout.addWidget(graph["widget"])
        elif not enabled and attr_id in self.graphs_by_id:
            self.graphs_by_id[attr_id]["widget"].deleteLater()
            del self.graphs_by_id[attr_id]

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
                try:
                    y.append(val.magnitude)
                except AttributeError:
                    y.append(val)
                data_line.setData(x, y)

    @QtCore.Slot()
    def double_click(self, item, column):
        if 1 == column:
            if (
                hasattr(item, "_tm_attribute") and
                hasattr(item._tm_attribute, "c_setter")
                and None != item._tm_attribute.c_setter
            ):
                item.setFlags(item.flags() | QtCore.Qt.ItemIsEditable)
                item._editing = True
            else:
                try:
                    item._tm_function()
                except AttributeError:
                    pass
        elif int(item._orig_flags) != int(item.flags()):
            item.setFlags(item._orig_flags)