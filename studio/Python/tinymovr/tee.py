from threading import Lock


class TFrame:
    def __init__(self, arbitration_id, data, ts=0):
        self.arbitration_id = arbitration_id
        self.data = bytes(data)
        self.ts = ts


class TBus:
    def recv(self, timeout=0):
        raise NotImplementedError()

    def send(self, frame):
        raise NotImplementedError()


class Tee:
    '''
    Filter and distribute incoming messages based on the boolean result
    of a filter callback.

    python-can does not allow filtering messages per recipient therefore
    this class.

    Also implements a simple forwarding mechanism for sending messages, to
    simplify interfacing with CAN bus objects.
    '''
    tees = []
    lock = Lock()

    def __init__(self, bus, filter_cb):
        self.bus = bus
        self.filter_cb = filter_cb
        self.queue = []
        self.tees.append(self)

    def recv(self):
        '''
        Tries to receive a message from the bus object and if successful,
        tests reception of each tee instance in the global index. 
        '''
        self.lock.acquire()
        response = None
        frame = self.bus.recv(1)
        if frame:
            for tee in self.tees:
                if tee.filter_cb(frame):
                    tee.queue.append(frame)
        try:
            response = self.queue.pop(0)
        except IndexError:
            pass
        self.lock.release()
        return response

    def send(self, frame):
        '''
        Send a frame by forwarding to the bus object
        '''
        self.lock.acquire()
        self.bus.send(frame)
        self.lock.release()
