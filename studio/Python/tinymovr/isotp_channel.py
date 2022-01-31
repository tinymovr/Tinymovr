
import threading
import time
from functools import cached_property
import serial.tools.list_ports as list_ports
import can

import isotp
from isotp import Address, IsoTpError
from avlos import Channel
from avlos.codecs import MultibyteCodec

from tinymovr.tee import Tee


CAN_EP_BITS = 6
ISOTP_TX_ADDR = 0x3E
ISOTP_RX_ADDR = 0x3F


can_devices = {
    "slcan": ("canable", "cantact"),
    "robotell": ("CP210",),
}


class ResponseError(Exception):

    def __init__(self, kw, *args, **kwargs):
        msg = "Node {} did not respond".format(kw)
        super().__init__(msg, *args, **kwargs)
        self.kw = kw


class OurCanStack(isotp.TransportLayer):

    def __init__(self, can_bus, address, *args, **kwargs):
        self.address = address
        self.tee = Tee(can_bus,
            lambda msg: msg.arbitration_id == self.address.rxid)
        isotp.TransportLayer.__init__(
            self,
            rxfn=self.rx_canbus,
            txfn=self.tx_canbus,
            address=address,
            *args, **kwargs
            )

    def tx_canbus(self, msg):
        can_msg = can.Message(
            arbitration_id=msg.arbitration_id,
            data = msg.data,
            is_extended_id=msg.is_extended_id,
            is_fd=msg.is_fd,
            bitrate_switch=msg.bitrate_switch
            )
        self.tee.send(can_msg)

    def rx_canbus(self):
        msg = self.tee.recv()
        isotp_msg = None
        if msg is not None:
            isotp_msg = isotp.CanMessage(arbitration_id=msg.arbitration_id, data=msg.data, extended_id=msg.is_extended_id, is_fd=msg.is_fd, bitrate_switch=msg.bitrate_switch)
        return isotp_msg

class ISOTPChannel(Channel):

    def __init__(self, can_bus, node_id, logger):
        super().__init__()
        self.node_id = node_id
        self.logger = logger
        self.stop_requested = False

        address = Address(isotp.AddressingMode.Normal_11bits,
                          rxid=gen_arbitration_id(node_id, ISOTP_RX_ADDR),
                          txid=gen_arbitration_id(node_id, ISOTP_TX_ADDR))
        self.stack = OurCanStack(can_bus, address)

        self.update_thread = threading.Thread(target=self.stack_update, daemon=True)
        self.update_thread.start()

    def send(self, buffer):
        self.stack.send(buffer)

    def shutdown(self):
        self.request_stop()
        self.update_thread.join()

    def recv(self, deadline=2.0, sleep_interval=0.01):
        total_interval = 0
        while total_interval < deadline:
            if self.stack.available():
                return self.stack.recv()
            time.sleep(sleep_interval)
            total_interval += sleep_interval
        raise ResponseError(self.node_id)

    def request_stop(self):
        self.stop_requested = True

    def stack_update(self):
        while False == self.stop_requested:
            self.stack.process()
            time.sleep(self.stack.sleep_time())

    def stack_error_handler(self, error: IsoTpError):
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
        arbitration_id=gen_arbitration_id(node_id, endpoint_id),
        is_extended_id=False,
        is_remote_frame=rtr,
        data=payload,
    )


def gen_arbitration_id(node_id: int, endpoint_id: int) -> int:
    """
    Generates a CAN id from node and endpoint ids
    """
    return (node_id << CAN_EP_BITS) | endpoint_id


def guess_channel(bustype_hint, logger):
    """
    Tries to guess a channel based on a bustype hint.
    """
    if "sim" == bustype_hint:
        return None
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