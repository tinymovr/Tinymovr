
import threading
import time
from functools import cached_property
import serial.tools.list_ports as list_ports
import can
from humanfriendly import parse_size
from isotp import CanStack, Address, IsoTpError, FlowControlTimeoutError, ConsecutiveFrameTimeoutError
from avlos import Channel, get_object_tree
from avlos.codecs import MultibyteCodec


CAN_EP_BITS = 6
CAN_EP_COUNT = 2 << CAN_EP_BITS
ISOTP_TX_ADDR = 0x3E
ISOTP_RX_ADDR = 0x3F


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


class ResponseError(Exception):

    def __init__(self, kw, *args, **kwargs):
        msg = "Node {} did not respond".format(kw)
        super().__init__(msg, *args, **kwargs)
        self.kw = kw


class ISOTPChannel(Channel):

    def __init__(self, can_bus, can_id, logger):
        super().__init__()
        self.logger = logger
        self.stop_requested = False
        self.can_id = can_id
        # First check device info using regular stack

        address = Address(0, rxid=(ISOTP_RX_ADDR + (can_id << CAN_EP_BITS)),
                          txid=(ISOTP_TX_ADDR + (can_id << CAN_EP_BITS)) )
        self.stack = CanStack(can_bus, address=address, error_handler=self.stack_error_handler)
        self.update_thread = threading.Thread(target=self.stack_update, daemon=True)
        self.update_thread.start()

    def send(self, buffer):
        self.stack.send(buffer)

    def recv(self, deadline=0.5, sleep_interval=0.02):
        if deadline > 0:
            total_interval = 0
            while not self.stack.available():
                time.sleep(sleep_interval)
                total_interval += sleep_interval
                if total_interval > deadline:
                    raise ResponseError(self.can_id)
        return self.stack.recv()

    def stack_update(self):
        while not self.stop_requested:
            self.stack.process()
            time.sleep(self.stack.sleep_time())

    def stack_error_handler(self, error: IsoTpError):
        if (isinstance(error, FlowControlTimeoutError) or
            isinstance(error, ConsecutiveFrameTimeoutError)):
            self.logger.error(error)
    
    @cached_property
    def serializer(self):
        return MultibyteCodec()


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