import time
import functools
import pint
from PySide2 import QtCore
from PySide2.QtCore import Signal
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
    QPushButton,
)
import pyqtgraph as pg
from tinymovr.constants import app_name
from tinymovr.channel import ResponseError as ChannelResponseError
from tinymovr.config import get_bus_config, configure_logging
from avlos import get_registry
from tinymovr.gui import Worker, format_value, load_icon


class MainWindow(QMainWindow):

    TreeItemCheckedSignal = Signal(dict)

    def __init__(self, app, arguments):
        super(MainWindow, self).__init__()

        # set units default format
        get_registry().default_format = ".6f~"

        self.start_time = time.time()
        self.logger = configure_logging()
        bitrate = int(arguments["--bitrate"])

        self.attribute_widgets_by_id = {}

        self.setWindowTitle(app_name)
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
        #self.right_frame.setMinimumWidth(820)

        main_layout = QHBoxLayout()
        main_layout.addWidget(self.left_frame)
        main_layout.addWidget(self.right_frame)
        main_layout.setSpacing(0)
        main_layout.setContentsMargins(0, 0, 0, 0)

        main_widget = QWidget()
        main_widget.setLayout(main_layout)
        main_widget.setMinimumHeight(600)
        self.setCentralWidget(main_widget)

        # pg.setConfigOptions(antialias=True)
        self.graphs_by_id = {}

        buses = arguments["--bus"].rsplit(sep=",")
        channel = arguments["--chan"]
        bitrate = int(arguments["--bitrate"])

        if not channel:
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
        self.worker.update_attrs.connect(self.update_attrs)
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

    def make_graph(self, attr):
        """
        Create a new pyqtgraph object, for the passed
        attribute. Return a dictionary containing the
        object and related data container.
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
        x = []
        y = []
        data_line = pg.PlotCurveItem(x, y, pen=pg.mkPen(width=1.00))
        graph_widget.addItem(data_line)
        return {
            "widget": graph_widget,
            "data": {"x": x, "y": y},
            "data_line": data_line,
        }

    @QtCore.Slot()
    def regen_tree(self, tms_by_id):
        """
        Regenerate the attribute tree
        """
        self.attribute_widgets_by_id = {}
        self.tree_widget.clear()
        all_items = []
        for name, node in tms_by_id.items():
            widget, items_list = self.parse_node(node, name)
            self.tree_widget.addTopLevelItem(widget)
            all_items.extend(items_list)
        for item in all_items:
            if hasattr(item, "_tm_function"):
                button = QPushButton("")
                button._tm_function = item._tm_function
                button.setIcon(load_icon("call.png"))
                self.tree_widget.setItemWidget(item, 1, button)
                button.clicked.connect(
                    functools.partial(self.function_call_clicked, item._tm_function)
                )
        header = self.tree_widget.header()
        header.setSectionResizeMode(QHeaderView.ResizeToContents)
        header.setStretchLastSection(False)

    def parse_node(self, node, name):
        widget = QTreeWidgetItem([name, 0, ""])
        widget._orig_flags = widget.flags()
        all_items = []
        try:
            # Let's assume it's a RemoteNode
            for attr_name, attr in node.remote_attributes.items():
                items, items_list = self.parse_node(attr, attr_name)
                widget.addChild(items)
                all_items.extend(items_list)
        except AttributeError:
            # Maybe a RemoteAttribute
            try:
                val = node.get_value()
                widget.setText(1, format_value(val))
                widget.setCheckState(0, QtCore.Qt.Unchecked)
                widget._tm_attribute = node
                widget._editing = False
                self.attribute_widgets_by_id[node.full_name] = {
                    "node": node,
                    "widget": widget,
                }
                all_items.append(widget)
            except AttributeError:
                # Must be a RemoteFunction then
                widget._tm_function = node
                all_items.append(widget)
        return widget, all_items

    @QtCore.Slot()
    def item_changed(self, item):
        if item._editing:
            # Value changed
            item._editing = False
            val = item.text(1)
            try:
                item._tm_attribute.set_value(get_registry()(val))
            except pint.PintError:
                try:
                    item._tm_attribute.set_value(float(val))
                except ValueError:
                    # Show pop up dialog
                    pass
            item.setText(1, format_value(item._tm_attribute.get_value()))

        # Checkbox changed
        enabled = item.checkState(0) == QtCore.Qt.Checked
        try:
            attr = item._tm_attribute
        except AttributeError:
            return
        attr_name = attr.full_name
        self.TreeItemCheckedSignal.emit({"attr": attr, "enabled": enabled})
        if enabled and attr_name not in self.graphs_by_id:
            graph = self.make_graph(attr)
            self.graphs_by_id[attr_name] = graph
            self.right_layout.addWidget(graph["widget"])
        elif not enabled and attr_name in self.graphs_by_id:
            self.graphs_by_id[attr_name]["widget"].deleteLater()
            del self.graphs_by_id[attr_name]

    @QtCore.Slot()
    def update_attrs(self, data):
        for attr_name, val in data.items():
            self.attribute_widgets_by_id[attr_name]["widget"].setText(
                1, format_value(val)
            )
            if attr_name in self.graphs_by_id:
                graph_info = self.graphs_by_id[attr_name]
                data_line = graph_info["data_line"]
                x = graph_info["data"]["x"]
                y = graph_info["data"]["y"]
                if len(x) >= 200:
                    x.pop(0)
                    y.pop(0)
                x.append(time.time() - self.start_time)
                if isinstance(val, pint.Quantity):
                    y.append(val.magnitude)
                else:
                    y.append(val)
                data_line.setData(x, y)
                # graph_info["widget"].getPlotItem().setXRange(x[0], x[-1])
                graph_info["widget"].update()
        self.status_label.setText(
            "{:.1f}Hz\t CH:{:.0f}%\t RT:{:.1f}ms".format(
                1 / self.worker.meas_dt,
                self.worker.load * 100,
                self.worker.rt_dt * 1000,
            )
        )

    @QtCore.Slot()
    def function_call_clicked(self, f):
        f()
        try:
            if f.meta["reload_data"]:
                time.sleep(0.1)
                self.worker.force_regen()
        except KeyError:
            pass

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
