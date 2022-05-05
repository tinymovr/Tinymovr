"""Tinymovr CLI

Usage:
    tinymovr [--bus=<bus>] [--bitrate=<bitrate>]
    tinymovr -h | --help
    tinymovr --version

Options:
    --bus=<bus>  One or more interfaces to use, first available is used [default: socketcan,slcan].
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
"""

import can
import pkg_resources
import IPython
from traitlets.config import Config
from docopt import docopt

from tinymovr.discovery import Discovery
from tinymovr.constants import app_name, base_node_name
from tinymovr.config import get_bus_config, configure_logging

"""
Copyright 2022 Ioannis Chatzikonstantinou (Tinymovr)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
"""


def spawn():
    """
    Spawns the Tinymovr Studio IPython-based CLI.
    """
    version = pkg_resources.require("tinymovr")[0].version
    arguments = docopt(__doc__, version=app_name + " " + str(version))

    logger = configure_logging()

    bustype, channel = get_bus_config(arguments["--bus"].rsplit(sep=","))
    bitrate = int(arguments["--bitrate"])
    bus = can.Bus(bustype=bustype, channel=channel, bitrate=bitrate)

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
    dsc = Discovery(bus, node_appeared, node_disappeared, logger)
    print("Listening for nodes...")

    c = Config()
    c.InteractiveShellApp.gui = "tk"
    c.TerminalIPythonApp.display_banner = False
    IPython.start_ipython(argv=[], config=c, user_ns=user_ns)
    logger.debug("Exiting...")
