
import logging
import serial
from serial.tools import list_ports

class IFace:
    
    def __init__(self, bus):
        self.bus = bus

ifaces_to_devices = {
    "slcan": "cantact",
    "robotell": "CP210"
}

def guess_channel(iface_hint):
    '''
    Tries to guess a channel based on an interface hint.
    '''
    device = ifaces_to_devices[iface_hint].lower()
    ports = ([
        p.device
        for p in serial.tools.list_ports.comports()
        if device in p.description.lower()
    ])
    if not ports:
        raise IOError("Could not autodiscover CAN channel")
    if len(ports) > 1:
        logging.warning('Multiple channels discovered - using the first')
    
    return ports[0]