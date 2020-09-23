import math
import can
from typing import Tuple, Dict, List
import serial
from serial.tools import list_ports
from tinymovr.iface import IFace


CAN_EP_SIZE: int = 6
CAN_EP_MASK: int = int(math.pow(2, CAN_EP_SIZE) - 1)

can_devices: Dict[str, tuple] = {
    "slcan": ("canable", "cantact"),
    "robotell": ("CP210", )
}


class CAN(IFace):
    '''
    Class implementing a CAN bus interface
    '''
    def __init__(self, bus):
        self.bus = bus

    def send(self, msg: can.Message):
        self.bus.send(msg)

    def send_new(self, node_id: int, endpoint_id: int,
                 rtr: bool=False, payload: bytearray=None):
        self.bus.send(create_frame(node_id, endpoint_id, rtr, payload))

    def receive(self, node_id: int, endpoint_id: int, timeout: float=0.05):
        frame_id: int = create_node_id(node_id, endpoint_id)
        frame: can.Message = self.bus.recv(timeout=timeout)
        if frame:
            if frame.arbitration_id == frame_id:
                return frame.data
            else:
                raise IOError("Received id mismatch: " +
                              str(frame.arbitration_id))
        else:
            raise TimeoutError()


def create_frame(node_id: int, endpoint_id: int,
                    rtr: bool=False, payload: bytearray=None) -> can.Message:
    '''
    Generate and return a CAN frame using python-can Message class
    '''
    return can.Message(arbitration_id=create_node_id(node_id, endpoint_id),
                       is_extended_id=False,
                       is_remote_frame=rtr,
                       data=payload)


def create_node_id(node_id: int, endpoint_id: int) -> int:
    '''
    Generate a CAN id from node and endpoint ids
    '''
    return (node_id << CAN_EP_SIZE) | endpoint_id


def extract_node_message_id(arbitration_id: int) -> Tuple[int, int]:
    node_id = arbitration_id >> CAN_EP_SIZE & 0xFF
    message_id = arbitration_id & CAN_EP_MASK
    return node_id, message_id


def guess_channel(iface_hint: str) -> str:
    '''
    Tries to guess a channel based on an interface hint.
    '''
    device_strings: List[str] = [s.lower() for s in
                                 can_devices[iface_hint]]
    ports: List[str] = []
    for p in serial.tools.list_ports.comports():
        desc_lower: str = p.description.lower()
        if any([s in desc_lower for s in device_strings]):
            ports.append(p.device)
    if not ports:
        raise IOError("Could not autodiscover CAN channel")
    if len(ports) > 1:
        logging.warning('Multiple channels discovered - using the first')

    return ports[0]