"""
Copyright 2022 Ioannis Chatzikonstantinou (Tinymovr)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
"""

from threading import Lock


class Tee:
    """
    Filter and distribute incoming messages based on the boolean result
    of a filter callback.

    python-can does not allow filtering messages per recipient therefore
    this class.

    Also implements a simple forwarding mechanism for sending messages, to
    simplify interfacing with CAN bus objects.
    """

    tees = []
    lock = Lock()

    def __init__(self, bus, filter_cb):
        self.bus = bus
        self.filter_cb = filter_cb
        self.queue = []
        self.tees.append(self)

    def recv(self, *args, **kwargs):
        """
        Tries to receive a message from the bus object and if successful,
        tests reception of each tee instance in the global index.
        """
        self.lock.acquire()
        response = None
        frame = self.bus.recv(*args, **kwargs)
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
        """
        Send a frame by forwarding to the bus object
        """
        self.lock.acquire()
        self.bus.send(frame)
        self.lock.release()
