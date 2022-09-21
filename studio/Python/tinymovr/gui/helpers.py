import os
import enum
import pint
from PySide2 import QtGui


def format_value(value, include_unit=True):
    if isinstance(value, enum.IntFlag):
        return str(value) if value > 0 else "(no flags)"
    if not include_unit and isinstance(value, pint.Quantity):
        return str(value.magnitude)
    if isinstance(value, float):
        return "{0:.6g}".format(value)
    return str(value)


def load_icon(fname_icon):
    path_this_dir = os.path.dirname(os.path.abspath(__file__))
    path_icons = os.path.join(path_this_dir, "..", "..", "resources", "icons")
    path_icon = os.path.join(path_icons, fname_icon)
    pixmap = QtGui.QPixmap(path_icon)
    icon = QtGui.QIcon()
    icon.addPixmap(pixmap, QtGui.QIcon.Normal)
    icon.addPixmap(pixmap, QtGui.QIcon.Disabled)
    return icon
