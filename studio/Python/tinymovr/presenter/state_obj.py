from typing import List
from enum import IntEnum

from tinymovr import ErrorIDs, error_descriptions

class StateObj():

    def __init__(self, data):
        self.data = data

    @property
    def state(self):
        return self.data[1]

    @property
    def mode(self):
        return self.data[2]

    @property
    def errors(self):
        if sum(self.data[-5:]) > 0:
            return [ErrorIDs(e) for e in self.data[-5:] if e > 0]
        elif (self.data[0] > 0):
            return [ErrorIDs(self.data[0])]
        return None

    @property
    def error_descriptions(self):
        if self.errors:
            return [error_descriptions[e] for e in self.errors]
        return None

    def __dir__(self):
        return ['state', 'mode', 'errors', 'error_descriptions']

