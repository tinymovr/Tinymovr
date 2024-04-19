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
    QCheckBox
)
from pint.errors import UndefinedUnitError
from avlos import get_registry
from avlos.datatypes import DataType
from tinymovr.gui.helpers import load_icon, load_pixmap, format_value, strtobool


class NodeTreeWidgetItem(QTreeWidgetItem):
    """
    NodeTreeWidgetItem: A specialized tree widget item class for managing hierarchical node structures.

    Designed to facilitate the addition of child nodes to a given tree widget, it ensures that the underlying node
    structure is accurately reflected within the tree widget interface.

    Inheritance:
    - Inherits from QTreeWidgetItem.

    Methods:
    - add_to_tree(tree_widget): Adds the current tree widget item to the provided tree widget and initializes its children.
    - _add_to_tree_cb(): Iteratively calls the '_add_to_tree_cb' method for each child node, enabling a recursive representation of the node hierarchy.
    """
    def __init__(self, name, *args, **kwargs):
        super().__init__([name, 0, ""], *args, **kwargs)

    def add_to_tree(self, tree_widget):
        tree_widget.addTopLevelItem(self)
        self._add_to_tree_cb()

    def _add_to_tree_cb(self):
        for i in range(self.childCount()):
            self.child(i)._add_to_tree_cb()


class EdgeTreeWidgetItem(QTreeWidgetItem):
    """
    EdgeTreeWidgetItem: A base tree widget item subclass for representing and managing nodes.

    Designed as a base class for other specialized tree widget items, it assists in presenting nodes
    within a QTreeWidget. Each node has a name and a summary, with the summary being set as a tooltip
    for the tree item.

    Inheritance:
    - Inherits from QTreeWidgetItem.

    Attributes:
    - _tm_node (Node): The node associated with this tree widget item.

    Methods:
    - __init__(name, node, *args, **kwargs): Initializes the tree widget item with the provided name and node.
    - _add_to_tree_cb(): A callback method meant to be overridden by subclasses for adding custom components to the tree.
    """

    def __init__(self, name, node, *args, **kwargs):
        super().__init__([name, 0, ""], *args, **kwargs)
        self._tm_node = node
        self.setToolTip(0, node.summary)

    def _add_to_tree_cb(self):
        pass


class AttrTreeWidgetItem(EdgeTreeWidgetItem):
    """
    AttrTreeWidgetItem: A tree widget item subclass designed for managing and presenting attributes.

    This widget item specializes in showing attributes and provides an interface for editing them,
    especially when they are of type `FLOAT`. It integrates with the `JoggableLineEdit` for
    floating-point attributes that support in-line jogging. For attributes that don't support
    jogging or aren't of type `FLOAT`, a standard QLineEdit is used.

    Inheritance:
    - Inherits from EdgeTreeWidgetItem.

    Attributes:
    - text_editor (QLineEdit or JoggableLineEdit): Editor for the attribute's value.
    - _checked (bool): A private attribute that maintains the checkbox state (for FLOAT types).

    Methods:
    - _add_to_tree_cb(): Adds the text editor to the tree widget. If the attribute is of type FLOAT, a checkbox is also added.
    - set_text(text): Sets the provided text to the text editor.
    - _on_editor_text_changed(): Slot to handle changes in the text editor. This method manages the process of setting the attribute value and triggers data reload if necessary.
    - _on_checkbox_changed(): Slot to handle checkbox state changes (used for FLOAT types).
    """

    def __init__(self, name, node, *args, **kwargs):
        super().__init__(name, node, *args, **kwargs)
        editable = (
            hasattr(self._tm_node, "setter_name") and self._tm_node.setter_name != None
        )
        if editable and node.dtype == DataType.FLOAT:
            self.text_editor = JoggableLineEdit(
                format_value(node.get_value()),
                editable,
                editable,
                node.meta.get("jog_step"),
            )
            self.text_editor.ValueChangedByJog.connect(self._on_editor_text_changed)
            self.text_editor.editingFinished.connect(self._on_editor_text_changed)
        else:
            self.text_editor = QLineEdit(format_value(node.get_value()))
            self.text_editor.editingFinished.connect(self._on_editor_text_changed)
        if not editable:
            self.text_editor.setReadOnly(True)
        self._checked = False

    def _add_to_tree_cb(self):
        self.treeWidget().setItemWidget(self, 1, self.text_editor)
        if self._tm_node.dtype == DataType.FLOAT:
            self.setCheckState(0, QtCore.Qt.Unchecked)

    def set_text(self, text):
        if not self.text_editor.hasFocus():
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
            self.treeWidget().window().worker.reset()
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
    """
    FuncTreeWidgetItem: A tree widget item subclass for managing and triggering functions.

    This widget item is specialized to present functions that can be invoked via a button.
    Once the button is clicked, the function associated with the widget item is executed.
    If the function requires arguments, an input dialog is presented to the user to collect them.

    Inheritance:
    - Inherits from EdgeTreeWidgetItem.

    Attributes:
    - None directly in this class. Inherits attributes from the superclass.

    Methods:
    - _add_to_tree_cb(): Adds a button with an icon to the tree widget. The button serves as the trigger to invoke the associated function.
    - _on_f_call_clicked(): Slot to handle button click events. This method manages the process of collecting function arguments and invoking the function. If the function has associated meta information indicating the need to reload data, a reset operation on the tree widget's worker is triggered.
    """

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
    """
    OptionsTreeWidgetItem: A tree widget item subclass for managing selectable options.

    This widget item is specialized to handle options in the form of a combo box.
    The combo box is populated with the provided options and integrated within the tree widget.
    The current selection index of the combo box is synchronized with the underlying node's value.

    Attributes:
    - combo_box_container (IconComboBoxWidget): A custom combo box widget containing an icon and a combo box.

    Inheritance:
    - Inherits from EdgeTreeWidgetItem.

    Methods:
    - _add_to_tree_cb(): Sets up and adds the combo box to the tree widget.
    - _on_combobox_changed(int): Slot to handle combo box index changes, synchronizes the new index with the underlying node's value.
    """

    def _add_to_tree_cb(self):
        self.combo_box_container = IconComboBoxWidget("call.png", self._tm_node.options)
        self.combo_box_container.combo.setCurrentIndex(self._tm_node.get_value())
        self.combo_box_container.combo.currentIndexChanged.connect(
            self._on_combobox_changed
        )
        self.treeWidget().setItemWidget(self, 1, self.combo_box_container)

    def set_text(self, value):
        self.combo_box_container.combo.setCurrentIndex(self._tm_node.options[value])

    @QtCore.Slot()
    def _on_combobox_changed(self, index):
        self._tm_node.set_value(index)
        self.combo_box_container.combo.setCurrentIndex(self._tm_node.get_value())


class BoolTreeWidgetItem(EdgeTreeWidgetItem):

    def __init__(self, name, node, *args, **kwargs):
        super().__init__(name, node, *args, **kwargs)
        self._checkbox = QCheckBox()
        # Set the initial state of the checkbox based on the attribute value
        self._checkbox.setChecked(node.get_value())
        if not self._tm_node.setter_name:
            self._checkbox.setEnabled(False)
        else:
            self._checkbox.stateChanged.connect(self._on_checkbox_state_changed)

    def _add_to_tree_cb(self):
        self.treeWidget().setItemWidget(self, 1, self._checkbox)

    def set_text(self, value):
        self._checkbox.setChecked(strtobool(value))

    @QtCore.Slot()
    def _on_checkbox_state_changed(self, state):
        # Convert the checkbox state to a boolean and set the attribute value
        value = state == QtCore.Qt.Checked
        self._tm_node.set_value(value)


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
    """
    A QLineEdit subclass that supports "jogging" (incremental adjustments) of its value
    using mouse movement.

    Features:
    - User can edit the text as a regular QLineEdit.
    - By holding the mouse button down for a short delay, the control enters jogging mode.
      Moving the mouse horizontally adjusts the value.
    - The increment step for jogging can be preset or will be determined based on the current value.

    Signals:
    - ValueChangedByJog: Emitted when the value is changed via jogging.

    Attributes:
    - editable (bool): Determines if the QLineEdit is editable when not jogging.
    - joggable (bool): Determines if the control supports jogging.
    - jogging (bool): Indicates if the control is currently in jogging mode.
    - jog_step (float or None): The increment step for jogging. If None, it's determined based on the value.

    Usage:
    editor = JoggableLineEdit(initial_text="0", editable=True, joggable=True)
    editor.ValueChangedByJog.connect(some_function)
    """

    ValueChangedByJog = Signal()

    def __init__(
        self,
        initial_text="0",
        editable=True,
        joggable=True,
        jog_step=None,
        *args,
        **kwargs,
    ):
        super().__init__(*args, **kwargs)
        self.editable = editable
        self.joggable = joggable
        self.jogging = False
        self.last_x = 0
        self.jog_step = jog_step
        self.current_jog_step = 0
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
        if self.jog_step:
            self.current_jog_step = self.jog_step
        else:
            text = self.text()
            try:
                value = float(text)
            except ValueError:
                value = get_registry()(text).magnitude
            self.current_jog_step = max(abs(value) * 0.01, 1e-6)
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
                    value = float(text)
                except ValueError:
                    value = get_registry()(text).magnitude
                value += self.current_jog_step * diff
                self.setText(str(value))
                self.ValueChangedByJog.emit()
            except ValueError:
                print("valueerror")
            self.last_x = event.x()
        else:
            self.jog_start_timer.stop()
            super().mouseMoveEvent(event)


class ArgumentInputDialog(QDialog):
    """
    A QDialog subclass that provides a dynamic form for user input based on a list of arguments.

    The dialog populates a QFormLayout with a QLabel and QLineEdit for each provided argument.
    It also appends Ok and Cancel buttons to finalize or dismiss the input.

    Features:
    - Each argument provided will be represented as a row with its name and data type.
    - User input can be retrieved as a dictionary using the `get_values` method.

    Attributes:
    - arguments (list): A list of objects with 'name' and 'dtype' attributes to represent each argument.
    - inputs (dict): A dictionary mapping argument names to their QLineEdit instances.

    Usage:
    dialog = ArgumentInputDialog(arguments)
    if dialog.exec_() == QDialog.Accepted:
        values = dialog.get_values()

    Args:
    - arguments (list): A list of objects where each object should have a 'name' and 'dtype' attribute.
    - parent (QWidget, optional): Parent widget for this dialog.

    Methods:
    - get_values(): Returns a dictionary mapping argument names to user inputs.

    """

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
