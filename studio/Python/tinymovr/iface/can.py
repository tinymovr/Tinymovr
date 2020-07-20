
import time
import can

from tinymovr.iface import IFace

import logging

CAN_EP_SIZE = 6

class CAN(IFace):
    '''
    Class implementing a CAN bus interface 
    '''
    def send(self, msg, delay=0.001):
        self.bus.send(msg)
        time.sleep(delay)

    def send_new(self, node_id, endpoint_id, rtr=False, payload=None):
        self.bus.send(self.create_frame(node_id, endpoint_id, rtr, payload))

    def receive(self, node_id, endpoint_id, timeout=0.2):
        frame_id = self.create_id(node_id, endpoint_id)
        frame = self.bus.recv(timeout=timeout)
        if frame:
            if frame.arbitration_id == frame_id:
                return frame.data
            else:
                raise IOError("Received id mismatch: " + str(frame.arbitration_id))
        else:
            raise TimeoutError()

    def create_frame(self, node_id, endpoint_id, rtr=False, payload=None):
        '''
        Generate and return a CAN frame using python-can Message class
        '''
        return can.Message(arbitration_id=self.create_id(node_id, endpoint_id),
                           is_extended_id=False,
                           is_remote_frame=rtr,
                           data=payload)

    def create_id(self, node_id, endpoint_id):
        '''
        Generate a CAN id from node and endpoint ids
        '''
        return (node_id << CAN_EP_SIZE) | endpoint_id
