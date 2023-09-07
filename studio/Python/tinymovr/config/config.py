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
from pathlib import Path
import logging
from importlib_resources import files
import can

from avlos.deserializer import deserialize
from tinymovr.codec import DataType
from tinymovr.channel import CANChannel

definitions = {"hash_uint32": {}, "name": {}}

for yaml_file in Path(files("tinymovr").joinpath("specs/")).glob("*.yaml"):
    with open(str(yaml_file)) as def_raw:
        definition = yaml.safe_load(def_raw)
        tmp_node = deserialize(definition)
        definitions["hash_uint32"][tmp_node.hash_uint32] = definition
        definitions["name"][definition["name"]] = definition


class ProtocolVersionError(Exception):
    def __init__(self, dev_id, version_str, *args, **kwargs):
        self.dev_id = dev_id
        self.version_str = cleanup_incomplete_version(version_str)
        msg = (
            "Incompatible protocol versions (hash mismatch) for device {}. "
            "Firmware is compatible with Studio version {}.\n\n"
            "Either upgrade studio and firmware, or install a compatible Studio version like so:\n\n"
            "pip3 uninstall tinymovr\npip3 install tinymovr=={}".format(
                self.dev_id, self.version_str, self.version_str
            )
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

    # Temporarily using a default definition to get the protocol_hash
    # This assumes that `protocol_hash` is standard across different definitions
    # Get the first definition as a temp
    tmp_definition = list(definitions["hash_uint32"].values())[0]
    node = deserialize(tmp_definition)
    node._channel = chan

    # Check for the correct definition using the remote hash
    protocol_hash = node.protocol_hash
    device_definition = definitions["hash_uint32"].get(protocol_hash)

    if not device_definition:
        raise ValueError(f"No device definition found for hash {protocol_hash}.")

    node = deserialize(device_definition)
    node._channel = chan
    if node.hash_uint32 != protocol_hash:
        raise ProtocolVersionError(node_id, "")

    return node


def create_device_with_hash_msg(heartbeat_msg):
    """
    Create a device, the heartbeat msg will be used
    to decode the actual hash value and id.
    """
    node_id = heartbeat_msg.arbitration_id & 0x3F
    chan = CANChannel(node_id)

    hash, *_ = chan.serializer.deserialize(heartbeat_msg.data[:4], DataType.UINT32)
    device_definition = definitions["hash_uint32"].get(hash)

    if not device_definition:
        raise ValueError(f"No device definition found for hash {hash}.")

    node = deserialize(device_definition)
    if node.hash_uint32 != hash:
        version_str = "".join([chr(n) for n in heartbeat_msg.data[4:]])
        if not version_str.strip():
            version_str = "1.3.1"
        raise ProtocolVersionError(node_id, version_str)

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


def cleanup_incomplete_version(version_str, char="."):
    """
    Clean up any version string that is
    incomplete or malformed
    """
    parts = version_str.split(char)

    while "" in parts:
        parts.remove("")

    return char.join(parts)
