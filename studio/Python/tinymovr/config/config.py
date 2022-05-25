"""
Copyright 2022 Ioannis Chatzikonstantinou (Tinymovr)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

def_path_str = str(
    importlib.resources.files("tinymovr").joinpath("config/device.yaml")
)
with open(def_path_str) as dev_def_raw:
    dev_def = yaml.safe_load(dev_def_raw)


class ProtocolVersionError(Exception):
    pass


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
    tee = Tee(
        bus,
        lambda msg: msg.arbitration_id >> CAN_EP_SIZE & 0xFF == node_id,
    )
    chan = CANChannel(node_id, tee)
    node = deserialize(dev_def)

    # We use the generated node to retrieve the hash from
    # the remote. This is ok as long as we know that the
    # hash endpoint will always be the 0th one. If there
    # is a hash mismatch, we raise an exception, otherwise
    # we return the device node as is.
    node._channel = chan
    if node.hash_uint32 != node.hash: # hash_uint32 is local, hash is remote
        raise ProtocolVersionError(
            "Incompatible protocol versions (hash mismatch).\nPlease try upgrading firmware & studio to the same version."
        )
    return node


def create_device_hash_msg(heartbeat_msg, bus):
    """
    Create a device, the hash msg will be used
    to decode the actual hash value
    """
    node_id = heartbeat_msg.arbitration_id & 0x3F
    tee = Tee(
        bus,
        lambda msg: msg.arbitration_id >> CAN_EP_SIZE & 0xFF == node_id,
    )
    chan = CANChannel(node_id, tee)
    node = deserialize(dev_def)
    hash, *_ = chan.serializer.deserialize(
            heartbeat_msg.data[:4], DataType.UINT32
        )
    if node.hash_uint32 != hash: # hash_uint32 is local, hash is remote
        raise ProtocolVersionError(
            "Incompatible protocol versions (hash mismatch).\nPlease try upgrading firmware & studio to the same version."
        )
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
