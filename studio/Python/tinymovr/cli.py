"""Tinymovr Studio CLI

Usage:
    tinymovr_cli [--bus=<bus>] [--chan=<chan>] [--bitrate=<bitrate>]
    tinymovr_cli -h | --help
    tinymovr_cli --version

Options:
    --bus=<bus>  One or more interfaces to use, first available is used [default: canine,slcan_disco].
    --chan=<chan>  The bus device "channel".
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
"""

import can
from canine import CANineBus
import pkg_resources
import IPython
from traitlets.config import Config
from docopt import docopt

from tinymovr import init_tee, destroy_tee
from tinymovr.discovery import Discovery
from tinymovr.constants import app_name, base_node_name
from tinymovr.config import get_bus_config, configure_logging

"""
Tinymovr CLI Module
Copyright Ioannis Chatzikonstantinou 2020-2023

The Tinymovr Studio IPython-based command line interface

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
    """
    Spawns the Tinymovr Studio IPython-based CLI.
    """
    version = pkg_resources.require("tinymovr")[0].version
    arguments = docopt(__doc__, version=app_name + " " + str(version))

    logger = configure_logging()

    buses = arguments["--bus"].rsplit(sep=",")
    channel = arguments["--chan"]
    bitrate = int(arguments["--bitrate"])

    if not channel:
        params = get_bus_config(buses)
        params["bitrate"] = bitrate
    else:
        params = {"bustype": buses[0], "channel": channel, "bitrate": bitrate}

    tms = {}
    user_ns = {}
    user_ns["tms"] = tms

    def node_appeared(node, node_id):
        node_name = "{}{}".format(base_node_name, node_id)
        print("Found {} with device id {}".format(node_name, node.uid))
        tms[node_id] = node
        user_ns[node_name] = node

    def node_disappeared(node_id):
        node_name = "{}{}".format(base_node_name, node_id)
        print("Lost {}".format(node_name))
        del tms[node_id]
        del user_ns[node_name]

    print(app_name + " " + str(version))

    init_tee(can.Bus(**params))
    dsc = Discovery(node_appeared, node_disappeared, logger)
    print("Listening for nodes...")

    c = Config()
    c.InteractiveShellApp.gui = "tk"
    c.TerminalIPythonApp.display_banner = False
    IPython.start_ipython(argv=[], config=c, user_ns=user_ns)
    logger.debug("Exiting...")
    destroy_tee()
