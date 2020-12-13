
from tinymovr.units import get_registry
from tinymovr.presenter import dict_obj

ureg = get_registry()


def present_default(attr, data, endpoint):
    if "units" in endpoint:
        data  = [v * ureg(u) for v, u in zip (data, endpoint["units"])]
    if attr.endswith("_asdict") and len(data) == 1:
        return dict_obj([strip_end(attr, "_asdict"), data[0]])
    elif len(data) == 1:    
        return data[0]
    else:
        return dict_obj(zip(endpoint["labels"], data))


def present_error(attr, data, endpoint):
    pass