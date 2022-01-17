
import can
from tinymovr.isotp_channel import VersionError, ResponseError

class Discovery(can.Listener):
    
    def __init__(self, can_bus_channel, appeared_cb, disappeared_cb, logger):
        self.logger = logger
        self.appeared_cb = appeared_cb
        self.disappeared_cb = disappeared_cb
        self.can_bus_channel = can_bus_channel
        self.notifier = can.Notifier(can_bus_channel, [self], timeout=1.0, loop=None)
        self.nodes = set()

    def on_message_received(self, message):
        arb_id = message.arbitration_id
        if 0x700 == arb_id & 0x700:
            node_id = arb_id & 0x3F
            if node_id not in self.nodes:
                if self.appeared_cb(node_id):
                    self.nodes.add(node_id)

    def on_error(self, error):
        print(error)
                