
tees = []

class Tee:
    '''
    Filter incoming messages based on a callback
    '''
    def __init__(self, can_bus, filter_cb):
        super().__init__()
        self.can_bus = can_bus
        self.filter_cb = filter_cb
        self.queue = []
        tees.append(self)

    def recv(self):
        msg = self.can_bus.recv(0)
        if msg:
            for tee in tees:
                if tee.filter_cb(msg):
                    tee.queue.append(msg)
        try:
            return self.queue.pop(0)
        except IndexError:
            return None

    def send(self, msg):
        self.can_bus.send(msg)
