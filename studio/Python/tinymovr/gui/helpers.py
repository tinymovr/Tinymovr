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
import os
import enum
import pint
from PySide6.QtCore import Qt
from PySide6 import QtGui
from PySide6.QtGui import QPixmap, QIcon, QGuiApplication, QPalette
from PySide6.QtWidgets import QMessageBox, QFileDialog, QTreeWidget
from avlos.definitions import RemoteAttribute
import tinymovr


app_stylesheet = """

/* --------------------------------------- QPushButton -----------------------------------*/

    QPushButton {
        background-color: #ededef;
        border-radius: 4px; 
        margin: 0 0 1px 0;
    }
    QPushButton:pressed {
        background-color: #cdcdcf;
        border-style: inset;
    }
    QPushButton:hover:!pressed
    {
    background-color: #eaeaec;
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
        background-color: #dfdfe1;      /* #605F5F; */
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
"""


app_stylesheet_dark = """

/* --------------------------------------- QPushButton -----------------------------------*/

    QPushButton {
        background-color: #363638;
        border-radius: 4px; 
        margin: 0 0 1px 0;
    }
    QPushButton:pressed {
        background-color: #767678;
        border-style: inset;
    }
    QPushButton:hover:!pressed
    {
    background-color: #464648;
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
        background-color: #dfdfe1;      /* #605F5F; */
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
"""


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
                (self.viewport().width() - self.placeholder_image.width()/pixel_ratio) / 2,
                (self.viewport().height() - self.placeholder_image.height()/pixel_ratio) / 2,
                self.placeholder_image,
            )
        else:
            super().paintEvent(event)


def format_value(value, include_unit=True):
    """
    Format a numeric value according to its
    type and return the formatted string
    """
    if isinstance(value, enum.IntFlag):
        return str(value) if value > 0 else "(no flags)"
    if not include_unit and isinstance(value, pint.Quantity):
        return str(value.magnitude)
    if isinstance(value, float):
        return "{0:.6g}".format(value)
    return str(value)


def load_pixmap(filename, dark_mode_suffix='_dark', high_dpi_suffix='@2x'):
    """
    Load an image from a file and return it as a QPixmap.
    Load appropriate variants based on pixel ratio and
    dark or light mode.
    """
    # Get the pixel ratio
    pixel_ratio = QtGui.QGuiApplication.primaryScreen().devicePixelRatio()

    # Prepare the filename based on the conditions
    parts = filename.split('.')
    if is_dark_mode():
        parts[0] += dark_mode_suffix
    if pixel_ratio > 1:
        parts[0] += high_dpi_suffix
    adjusted_filename = '.'.join(parts)

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


def hold_sema(sema):
    sema.acquire()
    try:
        yield
    finally:
        sema.release()


class TimedGetter:
    """
    An interface class that maintains timing
    information for the getter function
    """

    def __init__(self, error_handler):
        self.error_handler = error_handler
        self.dt = 0

    def get_value(self, getter):
        try:
            get_start_time = time.time()
            val = getter()
            get_dt = time.time() - get_start_time
            self.dt = self.dt * 0.99 + get_dt * 0.01
            return val
        except Exception as e:
            self.error_handler(e)


class RateLimitedFunction:
    """
    A class that limits the rate of calls to a passed
    function f
    """

    def __init__(self, func, rate):
        self.func = func
        self.rate = rate
        self.busy_dt = 0
        self.meas_dt = 0
        self.load = 0
        self.stop = False

    def call(self, *args, **kwargs):
        if self.busy_dt > 0 and self.busy_dt < self.rate:
            self.load = self.load * 0.99 + self.busy_dt / self.rate * 0.01
            time.sleep(self.rate - self.busy_dt)
            self.meas_dt = self.rate
        else:
            self.load = 1
            self.meas_dt = self.busy_dt
        start_time = time.time()
        self.func()
        self.busy_dt = time.time() - start_time

    def __call__(self, *args, **kwargs):
        self.call(self, *args, **kwargs)


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
    for _, attr in attr_dict.items():
        if isinstance(attr, RemoteAttribute):
            if "dynamic" in attr.meta and attr.meta["dynamic"] == True:
                dynamic_attrs.append(attr)
        elif hasattr(attr, "remote_attributes"):
            dynamic_attrs.extend(get_dynamic_attrs(attr.remote_attributes))
    return dynamic_attrs
