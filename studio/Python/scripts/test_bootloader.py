"""Tinymovr Bootloader Test Utility

Usage:
    test_bootloader --port=<port>
    test_bootloader -h | --help
    test_bootloader --version

Options:
    --port=<port>        (Virtual) COM Port to use
"""

'''
This script will check the state of the Qorvo Activeflashlight
bootloader that is included in the Tinymovr firmware.

The script will try to put the board in bootloader mode by
repeatedly sending a special character. If the board responds
with the character indicating bootloader mode, the script will
exit.
'''

from typing import Dict
import time
import serial
import pkg_resources
from docopt import docopt

shell_name: str = "Tinymovr Bootloader Test Utility"
send_val: bytes = b'\xAA'
recv_val: bytes = b'\x7E'

def main():
    version: str = pkg_resources.require("tinymovr")[0].version
    arguments: Dict[str, str] = docopt(__doc__, version=shell_name + ' ' + str(version))
    port: str = arguments['--port']
    with serial.Serial(port, 115200, timeout=0.005) as ser:
        while True:
            ser.write(send_val)
            data: bytes = ser.read(size=1)
            if data == recv_val:
                print("Correct token received, board is in bootloader mode")
                return
            elif len(data) == 1:
                print("Token received: " + str(data))
                

if __name__ == "__main__":
    main()