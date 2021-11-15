
import threading
import time
import serial.tools.list_ports as list_ports
import can
from humanfriendly import parse_size
from isotp import CanStack, Address, IsoTpError, FlowControlTimeoutError, ConsecutiveFrameTimeoutError
from avlos import Channel, get_object_tree


CAN_EP_BITS = 6
CAN_EP_COUNT = 2 << CAN_EP_BITS
CAN_DEVICE_INFO_ADDR = 0x03
CAN_TX_ADDR = 0x18
CAN_RX_ADDR = 0x19


min_fw_version = "0.8.10"


can_devices = {
    "slcan": ("canable", "cantact"),
    "robotell": ("CP210",),
}


class VersionError(Exception):
    
    def __init__(self, kw, found, required, *args, **kwargs):
        msg = "Node {} version incompatible (found: {}, required: {})".format(kw, found, required)
        super().__init__(msg, *args, **kwargs)
        self.kw = kw
        self.found = found
        self.required = required


class ISOTPChannel(Channel):

    def __init__(self, can_bus, can_id, logger):
        super().__init__()
        self.logger = logger
        # First check device info using regular stack

        address = Address(0, rxid=(CAN_RX_ADDR + (can_id << CAN_EP_BITS)),
                          txid=(CAN_TX_ADDR + (can_id << CAN_EP_BITS)) )
        self.stack = CanStack(can_bus, address=address, error_handler=self.stack_error_handler)
        self.update_thread = threading.Thread(target=self.stack_update, daemon=True)
        self.update_thread.start()

    def send(self, buffer):
        self.stack.send(buffer)

    def recv(self, deadline=0):
        return self.stack.recv()

    def stack_update(self):
        self.stack.process()
        time.sleep(self.stack.sleep_time())

    def stack_error_handler(self, error: IsoTpError):
        if (isinstance(error, FlowControlTimeoutError) or
            isinstance(error, ConsecutiveFrameTimeoutError)):
            pass
            # TODO: Display message
        

def discover_channels(path, callback, cancellation_token, logger):
    """
    Scans for CAN-ISOTP devices that match the path spec.
    This function blocks until cancellation_token is set.
    Channels spawned by this function run until channel_termination_token is set.
    """
    try:
        bustype, bitrate_string = path.split(":")
        bitrate = parse_size(bitrate_string)
        channel = guess_channel(bustype, logger)
    except (ValueError):
        raise Exception('"{}" is not a valid ISOTP channel. The format should be something like "isotp:slcan:1M".'
                    .format(path))

    bus = can.Bus(bustype=bustype, channel=channel, bitrate=bitrate)
    device_ids = [0x01]
    while not cancellation_token.is_set():
        for device_id in device_ids:
            try:
                bus.send(create_frame(device_id, CAN_DEVICE_INFO_ADDR, True))
                frame = bus.recv(timeout=0.2)
                data_len = len(frame.data)
            except TimeoutError:
                logger.debug("ISOTP device failed to respond")
            except AttributeError:
                logger.debug("ISOTP device returned empty response")
            else:
                callback(ISOTPChannel(bus, device_id, logger))


def create_frame(
    node_id: int, endpoint_id: int, rtr: bool = False, payload: bytearray = None
) -> can.Message:
    """
    Generates and return a CAN frame using python-can Message class
    """
    return can.Message(
        arbitration_id=create_node_id(node_id, endpoint_id),
        is_extended_id=False,
        is_remote_frame=rtr,
        data=payload,
    )


def create_node_id(node_id: int, endpoint_id: int) -> int:
    """
    Generates a CAN id from node and endpoint ids
    """
    return (node_id << CAN_EP_BITS) | endpoint_id


def guess_channel(bustype_hint, logger):
    """
    Tries to guess a channel based on a bustype hint.
    """
    device_strings = [s.lower() for s in can_devices[bustype_hint]]
    ports = []
    for p in list_ports.comports():
        desc_lower = p.description.lower()
        if any([s in desc_lower for s in device_strings]):
            ports.append(p.device)
    if not ports:
        raise IOError("Could not autodiscover CAN channel")
    if len(ports) > 1:
        logger.warning("Multiple channels discovered - using the first")

    return ports[0]