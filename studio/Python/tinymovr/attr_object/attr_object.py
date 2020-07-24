

class AttrObject:

    def __init__(self, keys, values):
        for key, value in zip(keys, values):
            if key is not None and value is not None:
                setattr(self, key, value)

    def __repr__(self):
        return str(self.__dict__)