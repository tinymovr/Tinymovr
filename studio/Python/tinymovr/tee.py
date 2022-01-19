
tees = []

class Tee:
    '''
    Filter and distribute incoming messages based on the boolean result
    of a filter callback.

    python-can does not allow filtering messages per recipient therefore
    this class.

    Also implements a simple forwarding mechanism for sending messages, to
    simplify interfacing with CAN bus objects.
    '''
    def __init__(self, can_bus, filter_cb):
        super().__init__()
        self.can_bus = can_bus
        self.filter_cb = filter_cb
        self.queue = []
        tees.append(self)

    def recv(self):
        '''
        Tries to receive a message from the bus object and if successful,
        tests reception of each tee instance in the global index. 
        '''
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
        '''
        Send a message by forwarding to the bus object
        '''
        self.can_bus.send(msg)
