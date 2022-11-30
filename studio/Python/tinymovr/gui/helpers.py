import os
import enum
import pint
from PySide2 import QtGui


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
