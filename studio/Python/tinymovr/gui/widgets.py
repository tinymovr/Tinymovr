"""
Tinymovr Studio custom Qt widgets
Copyright Ioannis Chatzikonstantinou 2020-2023

Various customized widgets for the Tinymovr Studio app

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

from functools import partial
from PySide6 import QtGui, QtCore
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import (
    QWidget,
    QTreeWidget,
    QTreeWidgetItem,
    QHBoxLayout,
    QLabel,
    QComboBox,
    QDialog,
    QVBoxLayout,
    QLabel,
    QLineEdit,
    QPushButton,
    QFormLayout,
    QHeaderView
)
from avlos import get_registry
from tinymovr.gui.helpers import load_icon, load_pixmap, format_value


class NodeTreeWidgetItem(QTreeWidgetItem):
    def __init__(self, name, *args, **kwargs):
        super().__init__([name, 0, ""], *args, **kwargs)
        self._orig_flags = self.flags()

    def add_to_tree(self, tree_widget):
        tree_widget.addTopLevelItem(self)
        self._add_to_tree_cb()

    def _add_to_tree_cb(self):
        for i in range(self.childCount()):
            self.child(i)._add_to_tree_cb()


class AttrTreeWidgetItem(NodeTreeWidgetItem):
    def __init__(self, name, node, *args, **kwargs):
        super().__init__(name, *args, **kwargs)
        self.setText(
            1, format_value(node.get_value())
        )
        self.setToolTip(0, node.summary)
        self.setCheckState(0, QtCore.Qt.Unchecked)
        self._tm_attribute = node
        self._editing = False
        self._checked = False


class FuncTreeWidgetItem(NodeTreeWidgetItem):
    def __init__(self, name, node, *args, **kwargs):
        super().__init__(name, *args, **kwargs)
        self._tm_node = node
        self.setToolTip(0, node.summary)

    def _add_to_tree_cb(self):
        button = QPushButton("")
        button.setIcon(load_icon("call.png"))
        self.treeWidget().setItemWidget(self, 1, button)
        button.clicked.connect(partial(self.f_call_clicked, self._tm_node))

    @QtCore.Slot()
    def f_call_clicked(self, f):
        args = []

        if f.arguments:
            dialog = ArgumentInputDialog(f.arguments, self.treeWidget())
            if dialog.exec_() == QDialog.Accepted:
                input_values = dialog.get_values()
                args = [input_values[arg.name] for arg in f.arguments]
            else:
                return  # User cancelled, stop the entire process
        args = [get_registry()(arg) for arg in args]

        f(*args)
        if "reload_data" in f.meta and f.meta["reload_data"]:
            self.worker.reset()


class OptionsTreeWidgetItem(NodeTreeWidgetItem):
    def __init__(self, name, node, *args, **kwargs):
        super().__init__(name, *args, **kwargs)
        self._tm_node = node
        self.setToolTip(0, node.summary)

    def _add_to_tree_cb(self):
        combo_box = IconComboBoxWidget("call.png", self._tm_node.options)
        self.treeWidget().setItemWidget(self, 1, combo_box)
        #combo_box.clicked.connect(partial(self.combo_item_selected, self._tm_node))


class PlaceholderQTreeWidget(QTreeWidget):
    """
    A custom QTreeWidget with support for displaying a placeholder image
    when the widget is empty.

    Attributes:
    - placeholder_image (QPixmap): An image displayed in the center of the
      widget when there are no top-level items.

    Public Methods:
    - paintEvent(event): Overrides the base class's paint event to paint the
      placeholder image if the widget is empty.

    Usage:
    - The placeholder image ("empty.png") is loaded upon initialization and is
      displayed with 50% opacity in the center of the widget when there are no
      top-level items.
    - If there are items present in the tree widget, it behaves like a standard
      QTreeWidget and displays the items without the placeholder.
    """

    def __init__(self, parent=None):
        super().__init__(parent)
        self.placeholder_image = load_pixmap("empty.png")

    def paintEvent(self, event):
        if self.topLevelItemCount() == 0:
            painter = QtGui.QPainter(self.viewport())
            painter.setOpacity(0.5)  # Adjust the opacity of the placeholder image
            pixel_ratio = QtGui.QGuiApplication.primaryScreen().devicePixelRatio()
            painter.drawPixmap(
                (self.viewport().width() - self.placeholder_image.width() / pixel_ratio)
                / 2,
                (
                    self.viewport().height()
                    - self.placeholder_image.height() / pixel_ratio
                )
                / 2,
                self.placeholder_image,
            )
        else:
            super().paintEvent(event)


class IconComboBoxWidget(QWidget):
    """
    A custom QWidget that combines an icon (QLabel with QPixmap) and a
    QComboBox into a single composite widget.

    Attributes:
    - combo (QComboBox): The embedded combo box that is part of this widget.

    Public Methods:
    - __init__(icon_path, enum_options, parent=None): Constructor to initialize the 
      composite widget with a specified icon, IntEnum options, and an optional parent widget.

    Usage:
    - This widget can be used when a visual cue (icon) needs to be placed
      directly beside a dropdown (QComboBox) in the UI.
    - The icon is loaded from the provided `icon_path` and is placed to the
      left of the combo box.
    """

    def __init__(self, icon_path, enum_options, parent=None):
        super(IconComboBoxWidget, self).__init__(parent)
        layout = QHBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)  # Remove margins
        layout.setSpacing(2)  # Small space between icon and combo box

        # Icon
        # icon_label = QLabel(self)
        # icon_label.setPixmap()
        # layout.addWidget(icon_label)

        # ComboBox
        self.combo = QComboBox(self)
        # Populate combo box with IntEnum options
        for option in enum_options:
            self.combo.addItem(option.name, option.value)
        layout.addWidget(self.combo)

        self.setLayout(layout)


class ArgumentInputDialog(QDialog):
    def __init__(self, arguments, parent=None):
        super(ArgumentInputDialog, self).__init__(parent)
        self.arguments = arguments
        self.inputs = {}

        layout = QFormLayout(self)

        # For each argument, create a label and input line edit
        for arg in arguments:
            label = QLabel(f"{arg.name} ({arg.dtype.nickname}):")
            line_edit = QLineEdit(self)
            layout.addRow(label, line_edit)
            self.inputs[arg.name] = line_edit

        # Add Ok and Cancel buttons
        button_layout = QVBoxLayout()
        ok_button = QPushButton("Ok", self)
        ok_button.clicked.connect(self.accept)
        cancel_button = QPushButton("Cancel", self)
        cancel_button.clicked.connect(self.reject)
        button_layout.addWidget(ok_button)
        button_layout.addWidget(cancel_button)

        layout.addRow(button_layout)
        self.setLayout(layout)

    def get_values(self):
        return {
            arg_name: line_edit.text() for arg_name, line_edit in self.inputs.items()
        }
