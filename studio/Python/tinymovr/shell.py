"""Tinymovr Shell Utility

Usage:
    tinymovr [--ids=<ids>] [--bustype=<bustype>] [--chan=<chan>] [--bitrate=<bitrate>]
    tinymovr -h | --help
    tinymovr --version

Options:
    --ids=<ids>          CAN node IDs to search [default: 1-10]
    --bustype=<bustype>  CAN bus type to use [default: slcan].
    --chan=<chan>        CAN channel (i.e. device) to use [default: auto].
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
"""

from typing import Dict
import logging
import pkg_resources
import can
import IPython

from docopt import docopt
import pynumparser

from tinymovr import UserWrapper
from tinymovr.iface import IFace
from tinymovr.iface.can import CAN, guess_channel

'''
This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
'''

shell_name = "Tinymovr Shell Utility"
base_name = "tm"


def spawn_shell():
    '''
    Spawns the Tinymovr Studio IPython shell.
    '''
    version: str = pkg_resources.require("tinymovr")[0].version
    arguments: Dict[str, str] = docopt(__doc__, version=shell_name + ' ' + str(version))

    logger = configure_logging()

    num_parser = pynumparser.NumberSequence(limits=(0, 16))
    node_ids = num_parser(arguments['--ids'])

    bustype: str = arguments['--bustype']
    channel: str = arguments['--chan']
    bitrate: int = int(arguments['--bitrate'])
    if channel == 'auto':
        channel = guess_channel(bustype_hint=bustype)
    can_bus: can.Bus = can.Bus(bustype=bustype,
                               channel=channel,
                               bitrate=bitrate)
    iface: IFace = CAN(can_bus)

    tms: Dict[str, UserWrapper] = {}
    for node_id in node_ids:
        try:
            tm: UserWrapper = UserWrapper(node_id=node_id, iface=iface)
            tm_name: str = base_name + str(node_id)
            logger.info("Connected to " + tm_name)
            tms[tm_name] = tm
        except TimeoutError:
            logger.info("Node " + str(node_id) + " timed out")
        except IOError:
            logger.error("Node " + str(node_id) +
                         " received abnormal message (possibly wrong ID?)")

    if len(tms) == 0:
        logger.error("No Tinymovr instances detected. Exiting shell...")
    else:
        tms_discovered: str = ", ".join(list(tms.keys()))
        tms["tms"] = list(tms.values())
        print(shell_name + ' ' + str(version))
        print("Discovered instances: " + tms_discovered)
        print("Access Tinymovr instances as tmx, where x \
is the index starting from 1")
        print("e.g. the first Tinymovr instance will be tm1.")
        print("Instances are also available by index in the tms list.")
        IPython.start_ipython(argv=["--no-banner"], user_ns=tms)
        logger.debug("Exiting shell...")


def configure_logging() -> logging.Logger:
    '''
    Configures logging options and
    generates a default logger instance.
    '''
    logging.getLogger('parso').setLevel(logging.WARNING)
    logging.getLogger('asyncio').setLevel(logging.WARNING)
    can.util.set_logging_level('warning')
    logger = logging.getLogger('tinymovr')
    logger.setLevel(logging.DEBUG)
    return logger
