
import pint

_registry = None

def get_registry():
    global _registry
    if not _registry:
        _registry = UnitRegistry()
        _registry.define('tick = turn / 8192')
    return _registry