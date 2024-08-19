"""
Usage:
    dfu.py --node_id=ID [--bin=PATH | --recovery] [--no-reset] [--force] [--bus=<bus>] [--chan=<chan>] [--bitrate=<bitrate>]

Options:
    --node_id=ID The CAN Node ID of the device in DFU mode.
    --bin=PATH   The path of the .bin file to upload.
    --recovery   Perform recovery procedure for inaccessible DFU bootloader.
    --no-reset   Do not perform a reset following successful flashing.
    --force      Force flashing even if device memory matches the .bin file.
    --bus=<bus>  One or more interfaces to use, first available is used [default: canine,slcan_disco].
    --chan=<chan>  The bus device "channel".
    --bitrate=<bitrate>  CAN bitrate [default: 1000000].
"""

import sys
import os
import time
import inspect
from pathlib import Path
import can
import yaml
from rich.progress import Progress
import IPython
from traitlets.config import Config
from docopt import docopt
from tinymovr.bus_router import init_router, destroy_router
from tinymovr.config import get_bus_config, create_device, configure_logging
from tinymovr.channel import ResponseError

"""
Tinymovr DFU Module
Copyright Ioannis Chatzikonstantinou 2020-2023

The Tinymovr Studio IPython-based command line interface

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

# Define constants
BIN_CHUNK_SIZE = 4
FLASH_START_ADDR = 0x00001000


def compare_bin_w_device(device, bin_path, string="Comparing"):
    """
    Compare .bin file and device non-volatile memory
    """
    with Progress() as progress:
        total_size = os.path.getsize(bin_path)  # Get the total size of .bin file
        task1 = progress.add_task("[green]{}...".format(string), total=total_size)

        with open(bin_path, "rb") as bin_file:
            chunk = bin_file.read(BIN_CHUNK_SIZE * 4)
            flash_addr = FLASH_START_ADDR
            while chunk:
                for i in range(BIN_CHUNK_SIZE):
                    file_value = int.from_bytes(
                        chunk[i * 4 : (i + 1) * 4], byteorder="little"
                    )
                    device_value = device.read_flash_32(flash_addr + i * 4)
                    if file_value != device_value:
                        return False
                chunk = bin_file.read(BIN_CHUNK_SIZE * 4)
                flash_addr += BIN_CHUNK_SIZE * 4
                progress.update(task1, advance=BIN_CHUNK_SIZE * 4)
    return True


def calculate_local_checksum(chunk):
    """Calculate the checksum for a chunk of data"""
    checksum = sum(
        int.from_bytes(chunk[i * 4 : (i + 1) * 4], byteorder="little")
        for i in range(BIN_CHUNK_SIZE)
    )
    return checksum & 0xFFFFFFFF  # To ensure we get a 32-bit value


def upload_bin(device, bin_path):
    """
    Upload a binary file to the device
    """
    total_size = os.path.getsize(bin_path)  # Get the total size of .bin file
    uploaded_size = 0

    print("\nErasing flash...")
    try:
        # Assume device.erase_all can take hash_validation and attempt to call it
        result = device.erase_all(device.hash_uint32)
    except TypeError:
        # Fallback to calling erase_all without hash_validation if TypeError is raised
        result = device.erase_all()
    if result != 0:
        print("\nError while erasing!")
        return
    print("Done.")
    
    with Progress() as progress:
        task2 = progress.add_task("[orange]Flashing...", total=total_size)
        with open(bin_path, "rb") as bin_file:
            # Read the first 128 bytes (32 bytes * 4)
            chunk = bin_file.read(BIN_CHUNK_SIZE * 4)
            flash_addr = FLASH_START_ADDR
            while chunk:
                # Write value to the scratchpad at index i
                for i in range(BIN_CHUNK_SIZE):
                    value = int.from_bytes(
                        chunk[i * 4 : (i + 1) * 4], byteorder="little"
                    )
                    device.write_scratch_32(i, value)
                    time.sleep(1e-5)

                # Commit the data in scratchpad to flash memory and get checksum
                try:
                    # Assume device.commit can take hash_validation and attempt to call it
                    device_checksum = device.commit(flash_addr, device.hash_uint32)
                except TypeError:
                    # Fallback to calling commit without hash_validation if TypeError is raised
                    device_checksum = device.commit(flash_addr)

                local_checksum = calculate_local_checksum(chunk)

                if device_checksum != local_checksum:
                    print(f"Checksum mismatch at address {flash_addr:08X}. Exiting...")
                    sys.exit(1)  # Exit the program

                chunk = bin_file.read(BIN_CHUNK_SIZE * 4)  # Read the next 128 bytes
                flash_addr += BIN_CHUNK_SIZE * 4  # Update the flash address
                uploaded_size += len(chunk)  # Update the uploaded size
                progress.update(task2, advance=BIN_CHUNK_SIZE * 4)


def spawn():
    # Parse command line arguments
    arguments = docopt(__doc__)
    node_id = int(arguments["--node_id"])
    bin_path = arguments["--bin"]

    # Set up the device
    buses = arguments["--bus"].rsplit(sep=",")
    channel = arguments["--chan"]
    bitrate = int(arguments["--bitrate"])

    logger = configure_logging()

    if not channel:
        params = get_bus_config(buses, bitrate=bitrate)
    else:
        params = {"bustype": buses[0], "channel": channel, "bitrate": bitrate}

    if arguments["--recovery"]:

        input("Please power off the device and then press any key to continue...")
        print("Now power on the device.")
        init_router(can.Bus, params, logger=logger, timeout=1.0)
        while True:
            try:
                device = create_device(node_id=node_id)
                print("The device is now in DFU mode.")
                break
            except ResponseError:
                pass
    else:
    
        init_router(can.Bus, params, logger=logger, timeout=1.0)
        device = create_device(node_id=node_id)

        if not bin_path:
            raise FileNotFoundError(f"No bin file specified!")

        # If a non-existing .bin file is specified, raise error
        elif bin_path and not Path(bin_path).is_file():
            raise FileNotFoundError(f"Bin file {bin_path} not found!")

        # If an existing .bin file is specified, upload it to the device
        elif bin_path:
            if (not arguments["--force"]) and compare_bin_w_device(device, bin_path):
                print("\nDevice memory matches the .bin file. Skipping flashing.")
            else:
                upload_bin(device, bin_path)
                compare_bin_w_device(device, bin_path, string="Verifying")
                if not arguments["--no-reset"]:
                    print("Resetting device...")
                    device.reset()
    destroy_router()


if __name__ == "__main__":
    spawn()
