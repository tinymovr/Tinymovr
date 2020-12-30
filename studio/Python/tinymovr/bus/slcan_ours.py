
import sys
import ctypes
import serial
from can.interfaces.slcan import slcanBus


class slcanBusOurs(slcanBus):
    '''
    A python-can slcan subclass that initializes the serial port
    on Windows systems with latency-optimized parameters.

    Otherwise identical to the slcanBus class.
    '''
    # Many thanks to jpieper (https://jpieper.com) for suggesting this.
    # Here is the original code:
    # https://github.com/mjbots/moteus/blob/main/lib/python/moteus/win32_aioserial.py

    def open(self) -> None:
        super().open()
        if sys.platform.startswith('win32'):
            import win32file
            # For Windows, many drivers treat all 0's as a special case that results
            # in better timings that are not possible to achieve through any other
            # means.
            timeouts = serial.win32.COMMTIMEOUTS()
            timeouts.ReadIntervalTimeout = 0
            timeouts.ReadTotalTimeoutMultiplier = 0
            timeouts.ReadTotalTimeoutConstant = 0
            timeouts.WriteTotalTimeoutMultiplier = 0
            timeouts.WriteTotalTimeoutConstant = 0
            serial.win32.SetCommTimeouts(self.serialPortOrig._port_handle, ctypes.byref(timeouts))