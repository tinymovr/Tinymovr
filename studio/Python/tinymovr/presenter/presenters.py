
from tinymovr.units import get_registry
from tinymovr.presenter import DictObj, StateObj

ureg = get_registry()


def present_default(attr, data, endpoint):
    if "units" in endpoint:
        data  = [v * ureg(u) for v, u in zip (data, endpoint["units"])]
    if attr.endswith("_asdict") and len(data) == 1:
        return DictObj([strip_end(attr, "_asdict"), data[0]])
    elif len(data) == 1:    
        return data[0]
    else:
        return DictObj(zip(endpoint["labels"], data))


def present_state(attr, data, endpoint):
    return StateObj(data)


def strip_end(text, suffix):
    if not text.endswith(suffix):
        return text
    return text[:len(text)-len(suffix)]