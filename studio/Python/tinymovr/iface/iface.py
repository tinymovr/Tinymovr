from typing import Dict, List
import logging
import serial
from serial.tools import list_ports

class IFace:
    
    def __init__(self, bus):
        self.bus = bus

ifaces_to_devices: Dict[str, tuple] = {
    "slcan": ("canable", "cantact"),
    "robotell": ("CP210", )
}

def guess_channel(iface_hint:str) -> str:
    '''
    Tries to guess a channel based on an interface hint.
    '''
    device_strings: List[str] = [s.lower() for s in ifaces_to_devices[iface_hint]]
    ports: List[str] = []
    for p in serial.tools.list_ports.comports():
        desc_lower:str = p.description.lower()
        if any([s in desc_lower for s in device_strings]):
            ports.append(p.device)
    if not ports:
        raise IOError("Could not autodiscover CAN channel")
    if len(ports) > 1:
        logging.warning('Multiple channels discovered - using the first')
    
    return ports[0]