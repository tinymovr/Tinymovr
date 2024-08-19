"""
Tinymovr GUI Helpers
Copyright Ioannis Chatzikonstantinou 2020-2023

Various GUI helper functions

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
from datetime import datetime
import os
import enum
import pint
from PySide6 import QtGui
from PySide6.QtGui import QGuiApplication, QPalette, QTextCursor
from PySide6.QtWidgets import QMessageBox, QFileDialog
from avlos.definitions import RemoteAttribute, RemoteEnum, RemoteBitmask
import pretty_errors
import tinymovr

app_stylesheet = """

/* --------------------------------------- QPushButton -----------------------------------*/

    QPushButton {
        background-color: #ededef;
        border-radius: 4px; 
        margin: 3px 10px 3px 0;
    }
    QPushButton:pressed {
        background-color: #cdcdcf;
        border-style: inset;
    }
    QPushButton:hover:!pressed
    {
    background-color: #eaeaec;
    } 

/* --------------------------------------- QComboBox -----------------------------------*/

    QComboBox {
        margin: 0 10px 0 5px;
    }

    QComboBox {
        margin: 0 10px 0 0;
    }

    QComboBox::drop-down {
        border: none;
        background-color: #ededef;
        border-radius: 4px; 
    }

/* --------------------------------------- QScrollBar -----------------------------------*/

    QScrollBar:horizontal
    {
        height: 15px;
        margin: 3px 15px 3px 15px;
        border: 1px transparent white;
        border-radius: 4px;
        background-color: white;
    }

    QScrollBar::handle:horizontal
    {
        background-color: #dfdfe1; 
        min-width: 5px;
        border-radius: 4px;
    }

    QScrollBar::add-line:horizontal
    {
        margin: 0px 3px 0px 3px;
        border-image: url(:/qss_icons/rc/right_arrow_disabled.png);
        width: 10px;
        height: 10px;
        subcontrol-position: right;
        subcontrol-origin: margin;
    }

    QScrollBar::sub-line:horizontal
    {
        margin: 0px 3px 0px 3px;
        border-image: url(:/qss_icons/rc/left_arrow_disabled.png);
        height: 10px;
        width: 10px;
        subcontrol-position: left;
        subcontrol-origin: margin;
    }

    QScrollBar::add-line:horizontal:hover,QScrollBar::add-line:horizontal:on
    {
        border-image: url(:/qss_icons/rc/right_arrow.png);
        height: 10px;
        width: 10px;
        subcontrol-position: right;
        subcontrol-origin: margin;
    }


    QScrollBar::sub-line:horizontal:hover, QScrollBar::sub-line:horizontal:on
    {
        border-image: url(:/qss_icons/rc/left_arrow.png);
        height: 10px;
        width: 10px;
        subcontrol-position: left;
        subcontrol-origin: margin;
    }

    QScrollBar::up-arrow:horizontal, QScrollBar::down-arrow:horizontal
    {
        background: none;
    }


    QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal
    {
        background: none;
    }

    QScrollBar:vertical
    {
        background-color: white;
        width: 15px;
        margin: 15px 3px 15px 3px;
        border: 1px transparent white;
        border-radius: 4px;
    }

    QScrollBar::handle:vertical
    {
        background-color: #dfdfe1;
        min-height: 5px;
        border-radius: 4px;
    }

    QScrollBar::sub-line:vertical
    {
        margin: 3px 0px 3px 0px;
        border-image: url(:/qss_icons/rc/up_arrow_disabled.png);
        height: 10px;
        width: 10px;
        subcontrol-position: top;
        subcontrol-origin: margin;
    }

    QScrollBar::add-line:vertical
    {
        margin: 3px 0px 3px 0px;
        border-image: url(:/qss_icons/rc/down_arrow_disabled.png);
        height: 10px;
        width: 10px;
        subcontrol-position: bottom;
        subcontrol-origin: margin;
    }

    QScrollBar::sub-line:vertical:hover,QScrollBar::sub-line:vertical:on
    {
        border-image: url(:/qss_icons/rc/up_arrow.png);
        height: 10px;
        width: 10px;
        subcontrol-position: top;
        subcontrol-origin: margin;
    }

    QScrollBar::add-line:vertical:hover, QScrollBar::add-line:vertical:on
    {
        border-image: url(:/qss_icons/rc/down_arrow.png);
        height: 10px;
        width: 10px;
        subcontrol-position: bottom;
        subcontrol-origin: margin;
    }

    QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical
    {
        background: none;
    }

    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical
    {
        background: none;
    }

    QAbstractScrollArea::corner {
        background: #dfdfe1;
        border: none;
    }
"""


app_stylesheet_dark = """

QSplitter::handle { background-color: black; }

/* --------------------------------------- QPushButton -----------------------------------*/

    QPushButton {
        background-color: #363638;
        border-radius: 4px; 
        margin: 3px 10px 3px 0;
    }
    QPushButton:pressed {
        background-color: #767678;
        border-style: inset;
    }
    QPushButton:hover:!pressed
    {
    background-color: #464648;
    }

/* --------------------------------------- QComboBox -----------------------------------*/

    QComboBox {
        margin: 0 10px 0 5px;
    }
    
    QComboBox::drop-down {
        border: none;
        background-color: #363638;
        border-radius: 4px; 
    }

    QComboBox::down-arrow
    {   
        border: 0px;
        background-repeat: no-repeat;
        background-position: center center;
        border-image: url(:/qss_icons/rc/down_arrow.png);
        height:20px;
        width:20px;
    }

/* ----------------------------------- Headers (dark only) -------------------------------*/

    QHeaderView::section {
        border-right-color: #262628;
        border-right-width: 1px;
        border-style: solid;
        margin: 0 4px;
    }

/* --------------------------------------- QScrollBar -----------------------------------*/

    QScrollBar:horizontal
    {
        height: 15px;
        margin: 3px 15px 3px 15px;
        border: 1px transparent white;
        border-radius: 4px;
        background-color: #363638;
    }

    QScrollBar::handle:horizontal
    {
        background-color: #605F5F; 
        min-width: 5px;
        border-radius: 4px;
    }

    QScrollBar::add-line:horizontal
    {
        margin: 0px 3px 0px 3px;
        border-image: url(:/qss_icons/rc/right_arrow_disabled.png);
        width: 10px;
        height: 10px;
        subcontrol-position: right;
        subcontrol-origin: margin;
    }

    QScrollBar::sub-line:horizontal
    {
        margin: 0px 3px 0px 3px;
        border-image: url(:/qss_icons/rc/left_arrow_disabled.png);
        height: 10px;
        width: 10px;
        subcontrol-position: left;
        subcontrol-origin: margin;
    }

    QScrollBar::add-line:horizontal:hover,QScrollBar::add-line:horizontal:on
    {
        border-image: url(:/qss_icons/rc/right_arrow.png);
        height: 10px;
        width: 10px;
        subcontrol-position: right;
        subcontrol-origin: margin;
    }


    QScrollBar::sub-line:horizontal:hover, QScrollBar::sub-line:horizontal:on
    {
        border-image: url(:/qss_icons/rc/left_arrow.png);
        height: 10px;
        width: 10px;
        subcontrol-position: left;
        subcontrol-origin: margin;
    }

    QScrollBar::up-arrow:horizontal, QScrollBar::down-arrow:horizontal
    {
        background: none;
    }


    QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal
    {
        background: none;
    }

    QScrollBar:vertical
    {
        background-color: #363638;
        width: 15px;
        margin: 15px 3px 15px 3px;
        border: 1px transparent white;
        border-radius: 4px;
    }

    QScrollBar::handle:vertical
    {
        background-color: #605F5F;
        min-height: 5px;
        border-radius: 4px;
    }

    QScrollBar::sub-line:vertical
    {
        margin: 3px 0px 3px 0px;
        border-image: url(:/qss_icons/rc/up_arrow_disabled.png);
        height: 10px;
        width: 10px;
        subcontrol-position: top;
        subcontrol-origin: margin;
    }

    QScrollBar::add-line:vertical
    {
        margin: 3px 0px 3px 0px;
        border-image: url(:/qss_icons/rc/down_arrow_disabled.png);
        height: 10px;
        width: 10px;
        subcontrol-position: bottom;
        subcontrol-origin: margin;
    }

    QScrollBar::sub-line:vertical:hover,QScrollBar::sub-line:vertical:on
    {
        border-image: url(:/qss_icons/rc/up_arrow.png);
        height: 10px;
        width: 10px;
        subcontrol-position: top;
        subcontrol-origin: margin;
    }

    QScrollBar::add-line:vertical:hover, QScrollBar::add-line:vertical:on
    {
        border-image: url(:/qss_icons/rc/down_arrow.png);
        height: 10px;
        width: 10px;
        subcontrol-position: bottom;
        subcontrol-origin: margin;
    }

    QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical
    {
        background: none;
    }

    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical
    {
        background: none;
    }

    QAbstractScrollArea::corner {
        background: #363638;
        border: none;
    }
"""


def format_value(value, include_unit=True):
    """
    Format a numeric value according to its
    type and return the formatted string
    """
    if isinstance(value, enum.IntFlag):
        return bitmask_string_representation(value) if value > 0 else "(no flags)"
    if isinstance(value, enum.IntEnum):
        return value.name
    if not include_unit and isinstance(value, pint.Quantity):
        return str(value.magnitude)
    if isinstance(value, float):
        return "{0:.6g}".format(value)
    return str(value)


def bitmask_string_representation(bitmask_value):
    labels_in_bitmask = [
        label.name for label in type(bitmask_value) if label & bitmask_value
    ]
    return ", ".join(labels_in_bitmask)


def load_pixmap(filename, dark_mode_suffix="_dark", high_dpi_suffix="@2x"):
    """
    Load an image from a file and return it as a QPixmap.
    Load appropriate variants based on pixel ratio and
    dark or light mode.
    """
    # Get the pixel ratio
    pixel_ratio = QtGui.QGuiApplication.primaryScreen().devicePixelRatio()

    # Prepare the filename based on the conditions
    parts = filename.split(".")
    if is_dark_mode():
        parts[0] += dark_mode_suffix
    if pixel_ratio > 1:
        parts[0] += high_dpi_suffix
    adjusted_filename = ".".join(parts)

    file_path = os.path.join(
        os.path.dirname(tinymovr.__file__), "resources", "icons", adjusted_filename
    )

    # Load the pixmap and set the device pixel ratio
    pixmap = QtGui.QPixmap(file_path)
    pixmap.setDevicePixelRatio(pixel_ratio)

    return pixmap


def is_dark_mode():
    return QGuiApplication.palette().color(QPalette.Window).lightness() < 128


def load_icon(fname_icon):
    """
    Load an image from a file and return it as a QIcon
    """
    pixmap = load_pixmap(fname_icon)
    icon = QtGui.QIcon()
    icon.addPixmap(pixmap, QtGui.QIcon.Normal)
    icon.addPixmap(pixmap, QtGui.QIcon.Disabled)
    return icon


def magnitude_of(val):
    """
    Extract the magnitude of a pint Quantity
    and return it, or return the value itself
    otherwise
    """
    if isinstance(val, pint.Quantity):
        return val.magnitude
    return val


class StreamRedirector(object):
    """
    A class to redirect writes from a stream to a QPlainTextEdit, including pretty_errors handling and timestamps.
    """
    def __init__(self, widget):
        self.widget = widget
        self.buffer = ''

    def write(self, message):
        # Timestamp the message
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        message_with_timestamp = f"[{timestamp}] {message}"

        # Redirect to the QPlainTextEdit widget
        self.widget.moveCursor(QtGui.QTextCursor.End)
        self.widget.insertPlainText(message_with_timestamp)
        self.widget.ensureCursorVisible()

    def flush(self):
        pass


class QTextBrowserLogger(logging.Handler):
    """A logging handler that directs logging output to a QTextBrowser widget."""
    def __init__(self, widget):
        super().__init__()
        self.widget = widget
        self.widget.setReadOnly(True)
        self.setFormatter(logging.Formatter('%(levelname)s: %(message)s'))

    def emit(self, record):
        msg = self.format(record)
        self.widget.append(self.colorize_message(record.levelno, msg))

    def colorize_message(self, level, message):
        color = {
            logging.DEBUG: "lightgreen",
            logging.INFO: "lightblue",
            logging.WARNING: "orange",
            logging.ERROR: "red",
            logging.CRITICAL: "purple"
        }.get(level, "black")
        timestamp = datetime.now().strftime('%H:%M:%S')
        return f'<font color="gray">[{timestamp}]</font> <font color="{color}">{message}</font>'
    

def configure_pretty_errors():
    pretty_errors.configure(
        separator_character='*',
        filename_display=pretty_errors.FILENAME_EXTENDED,
        line_number_first=True,
        display_link=True,
        lines_before=5,
        lines_after=2,
        line_color=pretty_errors.RED + '> ' + pretty_errors.default_config.line_color,
        code_color='  ' + pretty_errors.default_config.code_color,
        truncate_code=True,  # Truncate code lines to not overflow in the GUI
        display_locals=True
    )


class TimedGetter:
    """
    An interface class that maintains timing
    information for the getter function
    """

    def __init__(self):
        self.dt = 0

    def get_value(self, getter):
        get_start_time = time.time()
        val = getter()
        get_dt = time.time() - get_start_time
        self.dt = self.dt * 0.99 + get_dt * 0.01
        return val


def display_warning(title, text):
    """
    Display a pop up message with a warning
    """
    msg_box = QMessageBox()
    msg_box.setIcon(QMessageBox.Warning)
    msg_box.setText(text)
    msg_box.setWindowTitle(title)
    msg_box.exec_()


def display_file_open_dialog():
    """
    Display the file open dialog
    """
    documents_dir = os.path.expanduser("~/Documents")
    file_name, _ = QFileDialog.getOpenFileName(
        None, "Select JSON File", "", "JSON Files (*.json)"
    )
    return file_name


def display_file_save_dialog():
    """
    Display the file save dialog
    """
    documents_dir = os.path.expanduser("~/Documents")
    file_name, _ = QFileDialog.getSaveFileName(
        None, "Save JSON File", documents_dir, "JSON Files (*.json)"
    )
    return file_name


def check_selected_items(selected_items):
    if len(selected_items) == 0:
        display_warning(
            "Invalid Selection",
            "No Tinymovr nodes selected.\nSelect a single node",
        )
        return False
    elif len(selected_items) > 1:
        display_warning(
            "Invalid Selection",
            "Multiple Tinymovr nodes selected.\nSelect a single node",
        )
        return False
    return True


def get_dynamic_attrs(attr_dict):
    """
    Get the attributes that are marked as dynamic in the spec.
    """
    dynamic_attrs = []

    for attr in attr_dict.values():
        if isinstance(
            attr, (RemoteAttribute, RemoteEnum, RemoteBitmask)
        ) and attr.meta.get("dynamic"):
            dynamic_attrs.append(attr)
        elif hasattr(attr, "remote_attributes"):
            dynamic_attrs.extend(get_dynamic_attrs(attr.remote_attributes))

    return dynamic_attrs


def strtobool (val):
    """
    Convert a string representation of truth to true (1) or false (0).
    True values are 'y', 'yes', 't', 'true', 'on', and '1'; false values
    are 'n', 'no', 'f', 'false', 'off', and '0'.  Raises ValueError if
    'val' is anything else.
    """
    val = val.lower()
    if val in ('y', 'yes', 't', 'true', 'on', '1'):
        return 1
    elif val in ('n', 'no', 'f', 'false', 'off', '0'):
        return 0
    else:
        raise ValueError("invalid truth value %r" % (val,))