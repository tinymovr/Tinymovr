from threading import Semaphore


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
    sema = Semaphore()

    def __init__(self, can_bus, filter_cb):
        self.can_bus = can_bus
        self.filter_cb = filter_cb
        self.queue = []
        self.tees.append(self)

    def recv(self):
        '''
        Tries to receive a message from the bus object and if successful,
        tests reception of each tee instance in the global index. 
        '''
        self.sema.acquire()
        response = None
        msg = self.can_bus.recv(0)
        if msg:
            for tee in self.tees:
                if tee.filter_cb(msg):
                    tee.queue.append(msg)
        try:
            response = self.queue.pop(0)
        except IndexError:
            pass
        self.sema.release()
        return response

    def send(self, msg):
        '''
        Send a message by forwarding to the bus object
        '''
        self.sema.acquire()
        self.can_bus.send(msg)
        self.sema.release()
