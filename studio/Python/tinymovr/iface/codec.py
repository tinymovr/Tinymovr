
import struct
from enum import Enum

class DataType(Enum):
    INT8 = 0
    UINT8 = 1
    INT16 = 2
    UINT16 = 3
    INT32 = 4
    UINT32 = 5
    FLOAT = 6

class StructCodec():
    """
    Generic serializer/deserializer based on struct pack
    Adapted from:
    https://github.com/madcowswe/ODrive/blob/master/Firmware/fibre/python/fibre/remote_object.py
    """
    def __init__(self, struct_format, target_type):
        self._struct_format = struct_format
        self._target_type = target_type
    def get_length(self):
        return struct.calcsize(self._struct_format)
    def serialize(self, value):
        value = self._target_type(value)
        return struct.pack(self._struct_format, value)
    def deserialize(self, buffer):
        trimmed_buffer = buffer[:self.get_length()]
        value = struct.unpack(self._struct_format, trimmed_buffer)
        value = value[0] if len(value) == 1 else value
        return self._target_type(value)

codecs = {
    DataType.INT8 : StructCodec("<b", int),
    DataType.UINT8 : StructCodec("<B", int),
    DataType.INT16 : StructCodec("<h", int),
    DataType.UINT16 : StructCodec("<H", int),
    DataType.INT32 : StructCodec("<i", int),
    DataType.UINT32 : StructCodec("<I", int),
    DataType.FLOAT : StructCodec("<f", float)
}

class CANBusCodec:
    
    def serialize(self, values, *args):
        '''
        Serialize a series of variables to a
        CAN Bus-compatible bytes array
        '''
        buffer = bytearray()
        assert(len(values) == len(args))
        for value, dtype in zip(values, args):
            buffer.extend(codecs[dtype].serialize(value))
        return buffer


    def deserialize(self, data, *args):
        '''
        Deserialize a CAN Bus-compatible bytes
        array to a tuple of values
        '''
        index = 0
        values = []
        for dtype in args:
            values.append(codecs[dtype].deserialize(data[index:]))
            index += codecs[dtype].get_length()
        return values