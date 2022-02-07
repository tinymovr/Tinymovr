
import time
import threading

from avlos import ObjectFactory
from tinymovr.isotp_channel import ISOTPChannel, ResponseError
from tinymovr.tee import Tee


HEARTBEAT_BASE = 0x700

# Substitution strings
# The node names coming from Tinymovr are short to conserve memory.
# This can make them cryptic at times. We use a dictionary of shortened
# and original key-value pairs to make the names a bit more
# understandable
subst = {
    "ctrlr": "controller",
    "pos_est": "pos_estimate",
    "vel_est": "vel_estimate",
    "pos_set": "pos_setpoint",
    "vel_set": "vel_setpoint",
    "vel_lim": "vel_limit",
    "vel_I_gain": "vel_integrator_gain",
    "vel_I_band": "vel_integrator_deadband",
    "Iq_est": "Iq_estimate",
    "Iq_set": "Iq_setpoint",
    "Iq_lim": "Iq_limit",
    "pps": "pole_pairs",
    "calb": "calibrated",
    "dir": "direction"
}

class Discovery:
    '''
    Discover Tinymovr instances on the CAN bus using the periodically
    transmitted heartbeat frame.
    '''
    def __init__(self, bus, appeared_cb, disappeared_cb, logger, lost_timeout = 2.0):
        self.bus = bus
        self.logger = logger
        self.appeared_cb = appeared_cb
        self.disappeared_cb = disappeared_cb
        self.lost_timeout = lost_timeout

        self.active_nodes = {}
        self.update_stamps = {}
        self.pending_nodes = set()

        self.tee = Tee(bus, lambda msg: HEARTBEAT_BASE == msg.arbitration_id & HEARTBEAT_BASE)

        self.update_thread = threading.Thread(target=self.update, daemon=True)
        self.update_thread.start()

    def update(self):
        '''
        Periodically process pending messages.

        Note that this will receive only filtered messages from the
        correct tee, as such will not disrupt other receivers.
        '''
        while True:
            now = time.time()
            msg = self.tee.recv()
            while msg:
                node_id = msg.arbitration_id & 0x3F
                if node_id in self.active_nodes:
                    self.update_stamps[node_id] = now
                elif node_id not in self.pending_nodes:
                    self.pending_nodes.add(node_id)
                    chan = ISOTPChannel(self.bus, node_id, self.logger)
                    try:
                        f = ObjectFactory(chan, name_cb)
                        node = f.get_object_tree()
                        self.active_nodes[node_id] = node
                        self.update_stamps[node_id] = now
                        self.appeared_cb(node, node_id)
                    except ResponseError as e:
                        self.logger.error(e)
                    self.pending_nodes.remove(node_id)
                msg = self.tee.recv()
            
            # for_removal = set()
            # for node_id, stamp in self.update_stamps.items():
            #     if now - stamp > self.lost_timeout:
            #         for_removal.add(node_id)
            # for node_id in for_removal:
            #     del self.active_nodes[node_id]
            #     del self.update_stamps[node_id]
            #     self.disappeared_cb(node_id)
            time.sleep(0.1)


def name_cb(name):
    '''
    Callback for substituting names of object tree nodes
    '''
    try:
        return subst[name]
    except KeyError:
        return name
