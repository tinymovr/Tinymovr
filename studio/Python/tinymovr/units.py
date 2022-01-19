
import pint


_reg = None


def get_registry():
    global _reg
    if not _reg:
        _reg = pint.UnitRegistry()
        _reg.define('tick = turn / 8192')
    return _reg

# The dictionary below defines units for the various remote object
# attributes. The structure aims to match that of the remote object
# but it was decided to make this just a static structure independent
# of the dynamic spec, as units are secondary to Tinymovr operation.
attribute_units = {
    "controller": {
        "pos_est": _reg.tick,
        "vel_est": _reg.tick / _reg.second
    }
}