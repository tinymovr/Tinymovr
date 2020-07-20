"""
A text based interface. For example use over serial ports like
"/dev/ttyS1" or "/dev/ttyUSB0" on Linux machines or "COM1" on Windows.
The interface is a simple implementation that has been used for
recording CAN traces.
"""

import time
import logging
import struct

from tinymovr.singleton import Singleton

from can import BusABC, Message

START_CHAR = b'$'

try:
    import serial
except ImportError:
    logger.warning(
        "You won't be able to use the serial can backend without "
        "the serial module installed!"
    )
    serial = None

try:
    from serial.tools import list_ports
except ImportError:
    list_ports = None


class ArduinoCAN(BusABC, metaclass=Singleton):
    """
    Enable basic can communication over a serial device.

    .. note:: See :meth:`can.interfaces.serial.SerialBus._recv_internal`
              for some special semantics.

    """

    def __init__(self, channel, timeout=0.1, *args, **kwargs):
        """
            .. warning::
                Some serial port implementations don't care about the baudrate.

        :param float timeout:
            Timeout for the serial device in seconds (default 0.1).

        :param bool rtscts:
            turn hardware handshake (RTS/CTS) on and off

        """
        channel = "Arduino CAN"
        self.channel_info = "Serial interface: " + channel
        self.ser = self.get_serial_device(timeout)

        super().__init__(channel=channel, *args, **kwargs)
        time.sleep(2.0) # Needed for Arduino startup

    def shutdown(self):
        """
        Close the serial interface.
        """
        self.ser.close()

    def flush_tx_buffer(self):
        """
        Flush the outpub buffer.
        """
        self.ser.flushOutput()

    def send(self, msg, timeout=None):
        """
        Send a message over the serial device.

        :param can.Message msg:
            Message to send.

            .. note:: Flags like ``extended_id``, ``is_remote_frame`` and
                      ``is_error_frame`` will be ignored.

            .. note:: If the timestamp is a float value it will be converted
                      to an integer.

        :param timeout:
            This parameter will be ignored. The timeout value of the channel is
            used instead.

        """
        self.ser.flushInput()
        self.ser.flushOutput()
        try:
            a_id = struct.pack("<I", msg.arbitration_id)
        except struct.error:
            raise ValueError("Arbitration Id is out of range")
        byte_msg = bytearray(14)
        byte_msg[0] = ord(START_CHAR)
        for i in range(0, 4):
            byte_msg[i+1] = a_id[i]
        byte_msg[4] = byte_msg[4] | (64 if msg.is_remote_frame else 0)
        byte_msg[5] = msg.dlc 
        for i in range(0, 8):
            try:
                byte_msg[i+6] = msg.data[i]
            except IndexError:
                byte_msg[i+6] = 0
        self.ser.write(byte_msg)

    def _recv_internal(self, timeout):
        """
        Read a message from the serial device.

        :param timeout:

            .. warning::
                This parameter will be ignored. The timeout value of the channel is used.

        :returns:
            Received message and False (because not filtering as taken place).

            .. warning::
                Flags like is_extended_id, is_remote_frame and is_error_frame
                will not be set over this function, the flags in the return
                message are the default values.

        :rtype:
            Tuple[can.Message, Bool]
        """
        try:
            # ser.read can return an empty string
            # or raise a SerialException
            rx_byte = self.ser.read()
        except serial.SerialException:
            return None, False

        if rx_byte and ord(rx_byte) == ord(START_CHAR):
            
            s = bytearray(self.ser.read(4))
            arb_id = (struct.unpack("<I", s))[0]
            dlc = ord(self.ser.read())
            data = self.ser.read(8)

            msg = Message(time.time() * 1000,
                arbitration_id=arb_id,
                dlc=dlc,
                data=data,
            )
            return msg, False

        else:
            return None, False

    def get_serial_device(self, timeout):
        '''
        Searches for a port that includes the
        serial device (e.g. Arduino) string in its description
        '''
        arduino_ports = [
            p.device
            for p in serial.tools.list_ports.comports()
            if any(x in p.description for x in ('Arduino', 'Generic'))  # might be too optimistic...
        ]
        if not arduino_ports:
            raise IOError("Serial interface device not available")
        if len(arduino_ports) > 1:
            logging.warning('Multiple serial devices found - using the first')

        logging.debug(str(arduino_ports))
        return serial.Serial(arduino_ports[0], baudrate=500000, timeout=timeout)
