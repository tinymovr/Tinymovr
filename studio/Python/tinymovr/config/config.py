"""
Tinymovr Configuration Module
Copyright Ioannis Chatzikonstantinou 2020-2023

Implements functions to configure various aspects of Studio.

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

import yaml
import logging
from importlib_resources import files
import can

from avlos.deserializer import deserialize
from tinymovr.codec import DataType
from tinymovr.channel import CANChannel

dev_def = None

def_path_str = str(files("tinymovr").joinpath("config/device.yaml"))
with open(def_path_str) as dev_def_raw:
    dev_def = yaml.safe_load(dev_def_raw)


class ProtocolVersionError(Exception):
    def __init__(self, *args, **kwargs):
        msg = (
            "Incompatible protocol versions (hash mismatch)."
            "Please try upgrading firmware & studio to the same version."
        )
        super().__init__(msg, *args, **kwargs)


def get_bus_config(suggested_types=None):
    """
    Get the bus configuration (interface, channel) for
    the first of the suggested interface types. Present
    a pretty exception if none available.
    """
    configs = can.interface.detect_available_configs(suggested_types)
    try:
        return configs[0]
    except IndexError as exc:
        raise can.CanInitializationError("No active interface found") from exc


def create_device(node_id):
    """
    Create a device with the defined ID.
    The hash value will be retrieved from the remote.
    """
    chan = CANChannel(node_id)
    node = deserialize(dev_def)
    # We use the generated node to retrieve the hash from
    # the remote. This is ok as long as we know that the
    # hash endpoint will always be the 0th one. If there
    # is a hash mismatch, we raise an exception, otherwise
    # we return the device node as is.
    node._channel = chan
    # hash_uint32 is local, proto_hash is remote
    if node.hash_uint32 != node.protocol_hash:
        raise ProtocolVersionError()
    return node


def create_device_with_hash_msg(heartbeat_msg):
    """
    Create a device, the heartbeat msg will be used
    to decode the actual hash value and id
    """
    node_id = heartbeat_msg.arbitration_id & 0x3F
    chan = CANChannel(node_id)
    node = deserialize(dev_def)
    hash, *_ = chan.serializer.deserialize(heartbeat_msg.data[:4], DataType.UINT32)
    if node.hash_uint32 != hash:  # hash_uint32 is local, hash is remote
        raise ProtocolVersionError()
    node._channel = chan
    return node


def configure_logging():
    """
    Configure logging for various modules
    """
    logging.getLogger("can.io.logger").setLevel(logging.WARNING)
    logging.getLogger("parso").setLevel(logging.WARNING)
    logging.getLogger("asyncio").setLevel(logging.WARNING)
    logger = logging.getLogger("tinymovr")
    logger.setLevel(logging.DEBUG)
    return logger
