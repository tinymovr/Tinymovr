from typing import Dict, List
import struct
from enum import Enum


class DataType(Enum):
    VOID = 0
    BOOL = 1
    UINT8 = 2
    INT8 = 3
    UINT16 = 4
    INT16 = 5
    UINT32 = 6
    INT32 = 7
    UINT64 = 8
    INT64 = 9
    FLOAT = 10
    DOUBLE = 11


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

sizes: Dict[DataType, int] = {
    DataType.BOOL: 1,
    DataType.INT8: 1,
    DataType.UINT8: 1,
    DataType.INT16: 2,
    DataType.UINT16: 2,
    DataType.INT32: 4,
    DataType.UINT32: 4,
    DataType.FLOAT: 4,
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
