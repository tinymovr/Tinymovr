"""
Tinymovr Configuration Module
Copyright Ioannis Chatzikonstantinou 2020-2022

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

import time
import yaml
import logging
import importlib
import can

from avlos.deserializer import deserialize
from tinymovr.codec import DataType
from tinymovr.tee import Tee
from tinymovr.channel import CANChannel
from tinymovr.constants import CAN_EP_SIZE

dev_def = None

def_path_str = str(importlib.resources.files("tinymovr").joinpath("config/device.yaml"))
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
    the first of the suggested interface types
    """
    configs = can.interface.detect_available_configs(suggested_types)
    try:
        config = configs[0]
        return config["interface"], config["channel"]
    except IndexError as exc:
        raise can.CanInitializationError("No active interface found") from exc


def create_device(node_id, bus):
    """
    Create a device with the defined ID and bus.
    The hash value will be retrieved from the remote.
    """
    tee = Tee(bus, lambda msg: msg.arbitration_id >> CAN_EP_SIZE & 0xFF == node_id)
    chan = CANChannel(node_id, tee)
    node = deserialize(dev_def)

    # We use the generated node to retrieve the hash from
    # the remote. This is ok as long as we know that the
    # hash endpoint will always be the 0th one. If there
    # is a hash mismatch, we raise an exception, otherwise
    # we return the device node as is.
    node._channel = chan
    if (
        node.hash_uint32 != node.protocol_hash
    ):  # hash_uint32 is local, proto_hash is remote
        raise ProtocolVersionError()
    return node


def create_device_with_hash_msg(heartbeat_msg, bus):
    """
    Create a device, the hash msg will be used
    to decode the actual hash value
    """
    node_id = heartbeat_msg.arbitration_id & 0x3F
    tee = Tee(bus, lambda msg: msg.arbitration_id >> CAN_EP_SIZE & 0xFF == node_id)
    chan = CANChannel(node_id, tee)
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
