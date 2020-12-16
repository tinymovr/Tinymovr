from typing import List
from enum import IntEnum

from tinymovr import ErrorIDs, error_descriptions

class StateObj():

    def __init__(self, data):
        self.data = data

    @property
    def state(self):
        return 

    @property
    def mode(self):
        return self.mode

    @property
    def errors(self):
        state = self.tinymovr.state
        if sum(self.data[3:]) > 0:
            return [ErrorIDs[e] for e in self.data[3:] if e > 0]
        elif (self.data[0] > 0):
            return [ErrorIDs(self.data[0])]
        return None

    @property
    def error_descriptions(self):
        return [error_descriptions[e] for e in self.errors]

    def __dir__(self):
        return ['state', 'mode', 'errors', 'error_descriptions']

