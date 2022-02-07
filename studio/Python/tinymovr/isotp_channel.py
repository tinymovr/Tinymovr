
import threading
import time
from functools import cached_property
import serial.tools.list_ports as list_ports

import candle_driver
import isotp
from isotp import Address, IsoTpError
from avlos import Channel
from avlos.codecs import MultibyteCodec

from tinymovr.tee import Tee, TFrame, TBus


CAN_EP_BITS = 6
ISOTP_TX_ADDR = 0x3E
ISOTP_RX_ADDR = 0x3F


class ResponseError(Exception):
    def __init__(self, kw, *args, **kwargs):
        msg = "Node {} did not respond".format(kw)
        super().__init__(msg, *args, **kwargs)
        self.kw = kw


class LiteBus(TBus):
    def __init__(self, bitrate):
        devices = candle_driver.list_devices()

        try:
            self.device = devices[0]
        except IndexError:
            raise RuntimeError("No CANine (or Candlelight-enabled) devices found")
        self.device.open()
        self.ch = self.device.channel(0)
        self.ch.set_bitrate(bitrate)
        self.ch.start()

    def recv(self, timeout=0):
        try:
            _, can_id, can_data, _, ts = self.ch.read(timeout)
            return TFrame(can_id, can_data, ts)
        except TimeoutError:
            pass
        return None

    def send(self, frame):
        self.ch.write(frame.arbitration_id, frame.data)


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
        self.set_sleep_timing(0.001, 0.001)

    def tx_canbus(self, msg):
        self.tee.send(TFrame(msg.arbitration_id, msg.data))

    def rx_canbus(self):
        frame = self.tee.recv()
        if frame:
            return isotp.CanMessage(arbitration_id=frame.arbitration_id, data=frame.data)
        return None


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

    def recv(self, deadline=1.0, sleep_interval=0.001):
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


def gen_arbitration_id(node_id: int, endpoint_id: int) -> int:
    """
    Generates a CAN id from node and endpoint ids
    """
    return (node_id << CAN_EP_BITS) | endpoint_id
