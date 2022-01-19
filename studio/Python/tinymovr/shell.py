"""Tinymovr Shell Utility

Usage:
    tinymovr [--bustype=<bustype>] [--chan=<chan>] [--bitrate=<bitrate>] [--no-version-check]
    tinymovr -h | --help
    tinymovr --version

Options:
    --bustype=<bustype>  CAN bus type to use [default: slcan].
    --chan=<chan>        CAN channel (i.e. device) to use [default: auto].
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
    --no-version-check   Disable firmware-studio version compatibility check.
"""

import logging
import pkg_resources
import IPython
from traitlets.config import Config
from docopt import docopt

import can
from tinymovr.isotp_channel import guess_channel
from tinymovr.discovery import Discovery

"""
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

shell_name = "Tinymovr Studio"
base_name = "tm"


def spawn_shell():
    """
    Spawns the Tinymovr Studio IPython shell.
    """
    version = pkg_resources.require("tinymovr")[0].version
    arguments = docopt(__doc__, version=shell_name + " " + str(version))

    logging.getLogger("can.io.logger").setLevel(logging.WARNING)
    logging.getLogger("parso").setLevel(logging.WARNING)
    logging.getLogger("asyncio").setLevel(logging.WARNING)
    logger = logging.getLogger("tinymovr")
    logger.setLevel(logging.DEBUG)
    
    bustype = arguments["--bustype"]
    channel = arguments["--chan"]
    bitrate = int(arguments["--bitrate"])
    do_version_check = not arguments["--no-version-check"]
    if channel == "auto":
        channel = guess_channel(bustype, logger)
    can_bus = can.Bus(bustype=bustype, channel=channel, bitrate=bitrate)
    tms = {}
    user_ns = {}
    user_ns["tms"] = tms

    def node_appeared(node, node_id):
        node_name = "{}{}".format(base_name, node_id)
        print("Found {} with device id {}".format(node_name, node.uid))
        tms[node_id] = node
        user_ns[node_name] = node

    def node_disappeared(node_id):
        node_name = "{}{}".format(base_name, node_id)
        print("{} lost".format(node_name))
        del tms[node_id]
        del user_ns[node_name]
    
    print(shell_name + " " + str(version))
    dsc = Discovery(can_bus, node_appeared, node_disappeared, logger)
    print("Listening for nodes...")

    c = Config()
    c.InteractiveShellApp.gui = "tk"
    c.TerminalIPythonApp.display_banner = False
    IPython.start_ipython(argv=[], config=c, user_ns=user_ns)
    logger.debug("Exiting shell...")
