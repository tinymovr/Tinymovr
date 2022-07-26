# https://stackoverflow.com/questions/31875/is-there-a-simple-elegant-way-to-define-singletons?noredirect=1&lq=1


class Singleton(object):
    """Use to create a singleton"""

    def __new__(cls, *args, **kwds):
        """
        >>> s = Singleton()
        >>> p = Singleton()
        >>> id(s) == id(p)
        True
        """
        it_id = "__it__"
        # getattr will dip into base classes, so __dict__ must be used
        it = cls.__dict__.get(it_id, None)
        if it is not None:
            return it
        it = object.__new__(cls)
        setattr(cls, it_id, it)
        it.init(*args, **kwds)
        return it

    def init(self, *args, **kwds):
        pass
