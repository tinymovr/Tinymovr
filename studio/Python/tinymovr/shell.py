"""Tinymovr Shell Utility

Usage:
    tinymovr [--ids=<ids>] [--bustype=<bustype>] [--chan=<chan>] [--bitrate=<bitrate>] [--no-version-check]
    tinymovr -h | --help
    tinymovr --version

Options:
    --ids=<ids>          CAN node IDs to search [default: 1-2]
    --bustype=<bustype>  CAN bus type to use [default: slcan].
    --chan=<chan>        CAN channel (i.e. device) to use [default: auto].
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
    --no-version-check   Disable firmware-studio version compatibility check.
"""

from typing import Dict
import logging
import pkg_resources
import IPython
from traitlets.config import Config
from docopt import docopt
import pynumparser

import can
from tinymovr.isotp_channel import VersionError, ResponseError, ISOTPChannel, guess_channel
from avlos import get_object_tree

#from tinymovr import UserWrapper, VersionError
#from tinymovr.units import get_registry

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
    logger = make_logger()

    num_parser = pynumparser.NumberSequence(limits=(0, 16))
    node_ids = num_parser(arguments["--ids"])
    
    bustype = arguments["--bustype"]
    channel = arguments["--chan"]
    bitrate = int(arguments["--bitrate"])
    do_version_check = not arguments["--no-version-check"]
    if channel == "auto":
        channel = guess_channel(bustype, logger)
    can_bus = can.Bus(bustype=bustype, channel=channel, bitrate=bitrate)

    tms: Dict = {}
    for node_id in node_ids:
        try:
            isotp_channel = ISOTPChannel(can_bus, node_id, logger)
            root = get_object_tree(isotp_channel)
            #tm = UserWrapper(node_id=node_id, iface=iface, version_check=do_version_check)
            tm_name = base_name + str(node_id)
            logger.info("Connected to {}".format(tm_name))
            tms[tm_name] = root
        except TimeoutError:
            logger.info("Node {} timed out".format(node_id))
        except IOError as e:
            logger.error(str(e))
        except VersionError as e:
            logger.warning(str(e))
        except ResponseError as e:
            logger.info(str(e))

    if len(tms) == 0:
        logger.error("No Tinymovr instances detected. Exiting shell...")
    else:
        tms_discovered = ", ".join(list(tms.keys()))
        user_ns = {}
        user_ns.update(tms)
        user_ns["tms"] = list(tms.values())
        #user_ns["plot"] = plot
        #user_ns["ureg"] = get_registry()
        print(shell_name + " " + str(version))
        print("Discovered instances: " + tms_discovered)
        print(
            "Access Tinymovr instances as tmx, where x \
is the index starting from 1"
        )
        print("e.g. the first Tinymovr instance will be tm1.")
        print("Instances are also available by index in the tms list.")

        c = Config()
        c.InteractiveShellApp.gui = "tk"
        c.TerminalIPythonApp.display_banner = False
        IPython.start_ipython(argv=[], config=c, user_ns=user_ns)
        logger.debug("Exiting shell...")


def make_logger():
    """
    Configures logging options and
    generates a default logger instance.
    """
    logger = logging.getLogger("tinymovr")
    logger.setLevel(logging.DEBUG)
    return logger
