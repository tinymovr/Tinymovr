"""
Tinymovr Codecs Module
Copyright Ioannis Chatzikonstantinou 2020-2023

Implements codecs for various datatypes.

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

from typing import Dict, List
import struct
from avlos.datatypes import DataType


class Codec:
    """
    Generic codec base class
    """

    def serialize(self, value) -> bytes:
        raise NotImplementedError()

    def deserialize(self, buffer: bytes):
        raise NotImplementedError()


class StructCodec(Codec):
    """
    Python struct-based codec
    Adapted from:
    https://github.com/madcowswe/ODrive/blob/master/Firmware/fibre/python/fibre/remote_object.py
    """

    def __init__(self, struct_format, target_type: type):
        self._struct_format = struct_format
        self._target_type = target_type

    def get_length(self) -> int:
        return struct.calcsize(self._struct_format)

    def serialize(self, value) -> bytes:
        value = self._target_type(value)
        return struct.pack(self._struct_format, value)

    def deserialize(self, buffer: bytes):
        trimmed_buffer: bytes = buffer[: self.get_length()]
        value = struct.unpack(self._struct_format, trimmed_buffer)
        value = value[0] if len(value) == 1 else value
        return self._target_type(value)


codecs: Dict[DataType, StructCodec] = {
    DataType.BOOL: StructCodec("<?", bool),
    DataType.INT8: StructCodec("<b", int),
    DataType.UINT8: StructCodec("<B", int),
    DataType.INT16: StructCodec("<h", int),
    DataType.UINT16: StructCodec("<H", int),
    DataType.INT32: StructCodec("<i", int),
    DataType.UINT32: StructCodec("<I", int),
    DataType.FLOAT: StructCodec("<f", float),
}


class MultibyteCodec(Codec):
    def serialize(self, values, *args) -> bytearray:
        """
        Serialize a series of variables to a bytes array
        """
        buffer = bytearray()
        assert len(values) == len(args)
        for value, dtype in zip(values, args):
            buffer.extend(codecs[dtype].serialize(value))
        return buffer

    def deserialize(self, data, *args) -> List:
        """
        Deserialize a bytes array to a tuple of values
        """
        index: int = 0
        values = []
        for dtype in args:
            values.append(codecs[dtype].deserialize(data[index:]))
            index += codecs[dtype].get_length()
        return values
