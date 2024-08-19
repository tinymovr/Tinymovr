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


class IncompatibleSpecVersionError(Exception):
    def __init__(self, hash_value):
        self.hash_value = hash_value
        super().__init__(
            f"Device found, but incompatible (no device spec found for hash {hash_value})."
        )


specs = {}

# We added the following as a temporary solution
# after a hash was modified without modifying the
# spec (we are suspecting dependency upgrade
# breaking hash computation)
hash_aliases = {3526126264: [4118115615]}


def get_device_spec(hash, logger=None):
    if logger is None:
        logger = logging.getLogger("tinymovr")

    try:
        return specs["hash_uint32"][hash]
    except KeyError:
        for hash_alias in hash_aliases.get(hash, []):
            try:
                spec = specs["hash_uint32"][hash_alias]
                logger.debug(
                    "Found compatible spec via hash alias {} for device hash {}".format(
                        hash_alias, hash
                    )
                )
                return spec
            except KeyError:
                pass
    return None


def init_specs_dict():
    global specs
    specs = {"hash_uint32": {}}
    for yaml_file in Path(files("tinymovr").joinpath("specs/")).glob("*.yaml"):
        with open(str(yaml_file)) as def_raw:
            spec = yaml.safe_load(def_raw)
            add_spec(spec)


def add_spec(spec, logger=None):
    if logger is None:
        logger = logging.getLogger("tinymovr")

    tmp_node = deserialize(spec)
    hash_value = tmp_node.hash_uint32
    if hash_value in specs["hash_uint32"]:
        logger.warning(
            "Provided spec with hash {} already exists in hash/name dictionary".format(
                hash_value
            )
        )
    else:
        specs["hash_uint32"][hash_value] = spec


init_specs_dict()


def get_bus_config(suggested_types=None, bitrate=None):
    """
    Get the bus configuration (interface, channel) for
    the first of the suggested interface types. Present
    a pretty exception if none available.
    """
    configs = can.interface.detect_available_configs(suggested_types)
    try:
        config = configs[0]
        if bitrate:
            configs[0]["bitrate"] = bitrate
        return config
    except IndexError as exc:
        raise can.CanInitializationError("No active interface found") from exc


def create_device(node_id, logger=None):
    """
    Create a device with the defined ID.
    The hash value will be retrieved from the remote.
    """
    if logger is None:
        logger = logging.getLogger("tinymovr")

    # Temporarily using a default spec to get the protocol_hash
    # This assumes that `protocol_hash` is standard across different specs
    # Get the first spec as a temp
    tmp_hash = list(specs["hash_uint32"].keys())[0]
    tmp_spec = specs["hash_uint32"][tmp_hash]
    node = deserialize(tmp_spec)
    chan = CANChannel(node_id, 0)
    node._channel = chan

    # Check for the correct spec using the remote hash
    protocol_hash = node.protocol_hash
    device_spec = get_device_spec(protocol_hash)
    chan.compare_hash = protocol_hash & 0xFF

    if not device_spec:
        raise IncompatibleSpecVersionError(protocol_hash)

    node = deserialize(device_spec)
    node._channel = chan

    return node


def create_device_with_hash_msg(heartbeat_msg, logger=None):
    """
    Create a device, the heartbeat msg will be used
    to decode the actual hash value and id.
    """
    if logger is None:
        logger = logging.getLogger("tinymovr")

    node_id = heartbeat_msg.arbitration_id & 0x3F
    chan = CANChannel(node_id)

    hash, *_ = chan.serializer.deserialize(heartbeat_msg.data[:4], DataType.UINT32)
    device_spec = get_device_spec(hash)
    chan.compare_hash = hash & 0xFF

    if not device_spec:
        raise IncompatibleSpecVersionError(hash)

    node = deserialize(device_spec)

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
