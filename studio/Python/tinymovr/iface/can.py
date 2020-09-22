
import time
import can
from tinymovr.iface import IFace
import logging

CAN_EP_SIZE: int = 6


class CAN(IFace):
    '''
    Class implementing a CAN bus interface
    '''
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


def create_node_id(node_id: int, endpoint_id: int):
    '''
    Generate a CAN id from node and endpoint ids
    '''
    return (node_id << CAN_EP_SIZE) | endpoint_id
