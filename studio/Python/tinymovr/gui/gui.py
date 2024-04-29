"""Tinymovr Studio GUI

Usage:
    tinymovr [--bus=<bus>] [--chan=<chan>] [--spec=<spec>] [--bitrate=<bitrate>] [--max-timeouts=<count>]
    tinymovr -h | --help
    tinymovr --version

Options:
    --bus=<bus>  One or more interfaces to use, first available is used [default: canine,slcan_disco].
    --chan=<chan>  The bus device "channel".
    --spec=<spec> A custom device spec to be added to the list of discoverable specs.
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
    --max-timeouts=<count>  Max timeouts before nodes are rescanned [default: 5].
"""

import sys
import yaml
import pkg_resources
from importlib_resources import files
from docopt import docopt
from PySide6.QtWidgets import QApplication
from PySide6.QtGui import QIcon, QPixmap
from tinymovr.gui import MainWindow, app_stylesheet, app_stylesheet_dark, is_dark_mode
from tinymovr.constants import app_name
from tinymovr.config import configure_logging, add_spec


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


def spawn():
    version = pkg_resources.require("tinymovr")[0].version
    arguments = docopt(__doc__, version=app_name + " " + str(version))

    logger = configure_logging()

    spec_file = arguments["--spec"]
    if spec_file:
        with open(spec_file, 'r') as file:
            spec_data = yaml.safe_load(file)
            add_spec(spec_data, logger)

    app = QApplication(sys.argv)

    icon_path = files("tinymovr").joinpath("resources/icons/app_icon.png")
    app.setWindowIcon(QIcon(str(icon_path)))

    if is_dark_mode():
        app.setStyleSheet(app_stylesheet_dark)
    else:
        app.setStyleSheet(app_stylesheet)
    w = MainWindow(app, arguments, logger)

    w.show()
    sys.exit(app.exec_())
