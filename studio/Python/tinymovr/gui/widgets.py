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

from PySide6 import QtGui, QtCore
from PySide6.QtCore import Signal, QTimer
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
)
from pint.errors import UndefinedUnitError
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


class EdgeTreeWidgetItem(QTreeWidgetItem):
    def __init__(self, name, node, *args, **kwargs):
        super().__init__([name, 0, ""], *args, **kwargs)
        self._tm_node = node
        self._orig_flags = self.flags()
        self.setToolTip(0, node.summary)

    def _add_to_tree_cb(self):
        pass


class AttrTreeWidgetItem(EdgeTreeWidgetItem):
    def __init__(self, name, node, *args, **kwargs):
        super().__init__(name, node, *args, **kwargs)
        editable = hasattr(self._tm_node, "setter_name") and self._tm_node.setter_name != None
        if editable:
            self.text_editor = JoggableLineEdit(format_value(node.get_value()), editable, editable)
            self.text_editor.ValueChangedByJog.connect(self._on_editor_text_changed)
            self.text_editor.editingFinished.connect(self._on_editor_text_changed)
        else:
            self.text_editor = QLineEdit(format_value(node.get_value()))
        self._checked = False

    def _add_to_tree_cb(self):
        self.treeWidget().setItemWidget(self, 1, self.text_editor)
        self.setCheckState(0, QtCore.Qt.Unchecked)

    def set_text(self, text):
        self.text_editor.setText(text)

    @QtCore.Slot()
    def _on_editor_text_changed(self):
        attr = self._tm_node
        text = self.text_editor.text()
        try:
            attr.set_value(get_registry()(text))
        except UndefinedUnitError:
            attr.set_value(text)
        if "reload_data" in attr.meta and attr.meta["reload_data"]:
            self.worker.reset()
            return
        else:
            self.text_editor.setText(format_value(attr.get_value()))
    
    @QtCore.Slot()
    def _on_checkbox_changed(self):
        checked = self.checkState(0) == QtCore.Qt.Checked
        if checked != self._checked:
            self._checked = checked
            return True
        return False


class FuncTreeWidgetItem(EdgeTreeWidgetItem):
    def _add_to_tree_cb(self):
        button = QPushButton("")
        button.setIcon(load_icon("call.png"))
        self.treeWidget().setItemWidget(self, 1, button)
        button.clicked.connect(self._on_f_call_clicked)

    @QtCore.Slot()
    def _on_f_call_clicked(self):
        args = []
        f = self._tm_node

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
            self.treeWidget().window().worker.reset()


class OptionsTreeWidgetItem(EdgeTreeWidgetItem):
    def _add_to_tree_cb(self):
        self.combo_box_container = IconComboBoxWidget("call.png", self._tm_node.options)
        self.combo_box_container.combo.setCurrentIndex(self._tm_node.get_value())
        self.combo_box_container.combo.currentIndexChanged.connect(
            self._on_combobox_changed
        )
        self.treeWidget().setItemWidget(self, 1, self.combo_box_container)

    @QtCore.Slot()
    def _on_combobox_changed(self, index):
        self._tm_node.set_value(index)
        self.combo_box_container.combo.setCurrentIndex(self._tm_node.get_value())


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


class JoggableLineEdit(QLineEdit):

    ValueChangedByJog = Signal()

    def __init__(self, initial_text="0", editable=True, joggable=True, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.editable = editable
        self.joggable = joggable
        self.jogging = False
        self.last_x = 0
        self.setText(initial_text)
        self.setReadOnly(not editable)
        self.normal_cursor = self.cursor()
        
        self.jog_start_timer = QTimer(self)
        self.jog_start_timer.setSingleShot(True)
        self.jog_start_timer.timeout.connect(self.start_jog)

    def mousePressEvent(self, event):
        if self.joggable:
            self.jog_start_timer.start(500)
            self.last_x = event.x()
        super().mousePressEvent(event)

    def start_jog(self):
        self.setReadOnly(True)
        self.setCursor(QtGui.QCursor(QtGui.Qt.ClosedHandCursor))
        self.jogging = True

    def mouseReleaseEvent(self, event):
        if self.joggable:
            self.jog_start_timer.stop()
            
            self.setReadOnly(not self.editable)
            self.setCursor(self.normal_cursor)
            self.jogging = False
        super().mouseReleaseEvent(event)

    def mouseMoveEvent(self, event):
        if self.jogging:
            diff = event.x() - self.last_x
            text = self.text()
            try:
                try:
                    value = get_registry()(text)
                except UndefinedUnitError:
                    value = float(text)
                if value != 0:
                    value += value * diff * 0.02
                    self.setText(str(value))
                    self.ValueChangedByJog.emit()
            except ValueError:
                pass
            self.last_x = event.x()
        else:
            self.jog_start_timer.stop()
            super().mouseMoveEvent(event)


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
