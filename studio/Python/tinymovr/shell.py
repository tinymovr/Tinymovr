#!/usr/bin/env python3
"""Tinymovr Shell Utility

Usage:
    tinymovr [--iface=<iface>] [--chan=<chan>] [--bitrate=<bitrate>]
    tinymovr -h | --help
    tinymovr --version

Options:
    --iface=<iface>      CAN interface to use [default: slcan].
    --chan=<chan>        CAN channel (i.e. device) to use [default: auto].
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
"""

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

import logging
import pkg_resources
import can
import IPython

from docopt import docopt

from tinymovr.iface import guess_channel
from tinymovr import UserWrapper
from tinymovr.iface import CAN

shell_name = 'Tinymovr Shell Utility'
base_name = "tm"

def spawn_shell():
    '''
    Spawns the Tinymovr Studio IPython shell.
    '''
    version = pkg_resources.require("tinymovr")[0].version
    arguments = docopt(__doc__, version=shell_name + ' ' + str(version))
    
    logger = configure_logging()

    iface = arguments['--iface']
    chan = arguments['--chan']
    bitrate = int(arguments['--bitrate'])
    if chan == 'auto':
        chan = guess_channel(iface_hint=iface)
    iface = CAN(can.interface.Bus(interface=iface, channel=chan, bitrate=bitrate))

    tms = {}
    for node_id in range(1, 2):
        try:
            tm = UserWrapper(node_id=node_id, iface=iface)
            _ = tm.device_info
            tm_string = base_name+str(node_id)
            logger.info("Connected to " + tm_string)
            tms[tm_string] = tm
        except TimeoutError:
            logger.error("Node " + str(node_id) + " timed out")
        except IOError:
            logger.error("Node " + str(node_id) + " received abnormal message (possibly wrong ID?)")
    
    if len(tms) == 0:
        logger.error("No Tinymovr instances detected. Exiting shell...")
    else:
        tms["tms"] = list(tms.values())
        print(shell_name + ' ' + str(version))
        print("Access Tinymovr instances as tmx, where x is the index, e.g. tm1")
        print("Instances are also available by index in the tms list.")
        IPython.start_ipython(argv=["--no-banner"], user_ns=tms)
        logger.debug("Exiting shell...")

def configure_logging():
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

if __name__ == "__main__":
    main()
