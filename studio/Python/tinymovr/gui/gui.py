"""Tinymovr Studio GUI

Usage:
    tinymovr [--bus=<bus>] [--chan=<chan>] [--bitrate=<bitrate>]
    tinymovr -h | --help
    tinymovr --version

Options:
    --bus=<bus>  One or more interfaces to use, first available is used [default: canine,slcan_disco].
    --chan=<chan>  The bus device "channel".
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
"""

import sys
import pkg_resources
from docopt import docopt
from PySide2.QtWidgets import QApplication
from tinymovr.gui import MainWindow, app_stylesheet
from tinymovr.constants import app_name


"""
Tinymovr Studio GUI
Copyright Ioannis Chatzikonstantinou 2020-2023

The graphical user interface of Tinymovr Studio

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

# https://www.loekvandenouweland.com/content/pyside2-big-sur-does-not-show-window.html
import os
os.environ['QT_MAC_WANTS_LAYER'] = '1'

def spawn():
    version = pkg_resources.require("tinymovr")[0].version
    arguments = docopt(__doc__, version=app_name + " " + str(version))
    app = QApplication(sys.argv)
    app.setStyleSheet(app_stylesheet)
    w = MainWindow(app, arguments)
    w.show()
    sys.exit(app.exec_())
