"""Tinymovr Studio CLI

Usage:
    tinymovr_cli [--bus=<bus>] [--chan=<chan>] [--spec=<spec>] [--bitrate=<bitrate>]
    tinymovr_cli -h | --help
    tinymovr_cli --version

Options:
    --bus=<bus>  One or more interfaces to use, first available is used [default: canine,slcan_disco].
    --chan=<chan>  The bus device "channel".
    --spec=<spec> A custom device spec to be added to the list of discoverable spec.
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
"""

import yaml
import can
import pkg_resources
import IPython
from traitlets.config import Config
from docopt import docopt

from tinymovr import init_router, destroy_router
from tinymovr.device_discovery import DeviceDiscovery
from tinymovr.constants import app_name
from tinymovr.config import get_bus_config, configure_logging, add_spec

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

    spec_file = arguments["--spec"]
    if spec_file:
        with open(spec_file, 'r') as file:
            spec_data = yaml.safe_load(file)
            add_spec(spec_data, logger)

    buses = arguments["--bus"].rsplit(sep=",")
    channel = arguments["--chan"]
    bitrate = int(arguments["--bitrate"])

    if not channel:
        params = get_bus_config(buses, bitrate=bitrate)
    else:
        params = {"bustype": buses[0], "channel": channel, "bitrate": bitrate}

    tms = {}
    user_ns = {}
    user_ns["tms"] = tms

    def node_appeared(node, node_id):
        display_name = "{}{}".format(node.name, node_id)
        print("Found {} with device uid {}".format(display_name, node.uid))
        tms[node_id] = node
        user_ns[display_name] = node

    def node_disappeared(node_id):
        display_name = "{}{}".format(tms[node_id].name, node_id)
        print("Lost {}".format(display_name))
        del user_ns[display_name]
        del tms[node_id]

    print(app_name + " " + str(version))

    #TODO: router init should not happen in CLI spawn function
    init_router(can.Bus, params, logger=logger)
    dsc = DeviceDiscovery(node_appeared, node_disappeared, logger)
    print("Listening for nodes...")

    c = Config()
    c.TerminalIPythonApp.display_banner = False
    IPython.start_ipython(argv=[], config=c, user_ns=user_ns)
    logger.debug("Exiting...")
    destroy_router()


if __name__ == "__main__":
    spawn()
