
import time
import threading
from tinymovr.tee import Tee

class Discovery:
    
    def __init__(self, can_bus, appeared_cb, disappeared_cb, logger):
        self.logger = logger
        self.appeared_cb = appeared_cb
        self.disappeared_cb = disappeared_cb
        self.nodes = set()

        self.tee = Tee(can_bus, lambda msg: 0x700 == msg.arbitration_id & 0x700)

        self.update_thread = threading.Thread(target=self.stack_update, daemon=True)
        self.update_thread.start()

    def stack_update(self):
        while True:
            msg = self.tee.recv()
            if msg:
                node_id = msg.arbitration_id & 0x3F
                if node_id not in self.nodes:
                    if self.appeared_cb(node_id):
                        self.nodes.add(node_id)
            time.sleep(0.5)
