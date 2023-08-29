from PySide6 import QtGui
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import (
    QWidget,
    QTreeWidget,
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

from tinymovr.gui.helpers import load_pixmap


class OurQTreeWidget(QTreeWidget):
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
    def __init__(self, icon_path, parent=None):
        super(IconComboBoxWidget, self).__init__(parent)
        layout = QHBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)  # Remove margins
        layout.setSpacing(2)  # Small space between icon and combo box

        # Icon (adjust the path to your icon)
        icon_label = QLabel(self)
        pixmap = QPixmap(icon_path)
        icon_label.setPixmap(pixmap)
        layout.addWidget(icon_label)

        # ComboBox
        self.combo = QComboBox(self)
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
