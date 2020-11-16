
''' Tinymovr base module.

This module includes the base Tinymovr class that implements the API
to interface with the Tinymovr motor control board.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
'''

from copy import copy
import json
#from pkg_resources import parse_version
from tinymovr.iface import IFace
from tinymovr.objdict import objdict
from tinymovr.units import get_registry
from pint import Quantity as _Q

ureg = get_registry()

class Tinymovr:

    def __init__(self, node_id: int, iface: IFace):
        self.node_id: int = node_id
        self.iface: IFace = iface

        di = self.device_info
        self.fw_version = '.'.join([str(di.fw_major),
                                   str(di.fw_minor), str(di.fw_patch)])

        #parse_version(self.fw_version) >= parse_version(value["from_version"]

    def __getattr__(self, _attr: str):
        attr = strip_end(_attr, "_asdict")
        eps = self.iface.get_ep_map()
        codec = self.iface.get_codec()
        if attr in eps:
            d = eps[attr]

            if d["type"] == "w":
                # This is a write-type endpoint
                def wrapper(*args, **kwargs):
                    assert(len(args) == 0 or len(kwargs) == 0)
                    if len(kwargs) > 0:
                        assert("labels" in d)
                        inputs = [kwargs[k] if k in kwargs else
                                  d["defaults"][k] for k in d["labels"]]
                    elif len(args) > 0:
                        inputs = [args[i] if i < len(args) else
                                  d["defaults"][k] for i, k in enumerate(d["labels"])]
                    else:
                        inputs = []
                    if "units" in d:
                        inputs = [v.to(d["units"][i]).magnitude if isinstance(v, _Q)
                                  else v for i, v in enumerate(inputs)]
                    payload=None
                    if len(inputs) > 0:                        
                        payload = codec.serialize(inputs, *d["types"])
                    self.iface.send(self.node_id, d["ep_id"], payload=payload)

                return wrapper

            elif d["type"] == "r":
                # This is a read-type endpoint
                self.iface.send(self.node_id, d["ep_id"])
                response = self.iface.receive(self.node_id, d["ep_id"])
                outputs = codec.deserialize(response, *d["types"])
                if "units" in d:
                    outputs  = [v * ureg(u) for v, u in zip (outputs, d["units"])]
                if _attr.endswith("_asdict") and len(outputs) == 1:
                    return {attr: outputs[0]}
                elif len(outputs) == 1:    
                    return outputs[0]
                else:
                    return objdict(zip(d["labels"], outputs))

    def __dir__(self):
        return list(self.iface.get_ep_map().keys())

    def calibrate(self):
        self.set_state(1)

    def idle(self):
        self.set_state(0)

    def position_control(self):
        self.set_state(2, 2)

    def velocity_control(self):
        self.set_state(2, 1)

    def current_control(self):
        self.set_state(2, 0)

    def export_config(self, file_path: str):
        '''
        Export the board config to a file
        '''
        config_map = {}
        for k, v in self.iface.get_ep_map().items():
            if v["type"] == 'r' and "ser_map" in v:
                # Node can be serialized (saved)
                vals = getattr(self, k)
                config_map.update(
                    self._data_from_arguments(vals, v["ser_map"]))
        with open(file_path, 'w') as f:
            json.dump(config_map, f)

    def restore_config(self, file_path: str):
        '''
        Restore the board config from a file
        '''
        with open(file_path, 'r') as f:
            data = json.load(f)
        for k, v in self.iface.get_ep_map().items():
            if v["type"] == 'w' and "ser_map" in v:
                # Node has saved data and can be deserialized (restored)
                kwargs = self._arguments_from_data(v["ser_map"], data)
                if len(kwargs):
                    f = getattr(self, k)
                    f(**kwargs)

    def _data_from_arguments(self, args, ep_map):
        '''
        Generate a nested dictionary from a dictionary of values,
        following the template in ep_map
        '''
        data = {}
        for key, value in ep_map.items():
            if isinstance(value, dict):
                data[key] = self._data_from_arguments(args, value)
            elif isinstance(value, tuple):
                data[key] = {k: getattr(args, k) for k in value}
            else:
                raise TypeError("Map is not a dictionary or tuple")
        return data

    def _arguments_from_data(self, ep_map, ep_data):
        '''
        Generate a flat argument dictionary from a nested dictionary
        containing values for keys in endpoint labels
        '''
        kwargs = {}
        if isinstance(ep_map, dict) and isinstance(ep_data, dict):
            for key, value in ep_map.items():
                if key in ep_data:
                    kwargs.update(
                        self._arguments_from_data(value, ep_data[key]))
        elif isinstance(ep_map, tuple) and isinstance(ep_data, dict):
            for key in ep_map:
                if key in ep_data:
                    kwargs[key] = ep_data[key]
        else:
            raise TypeError("Mismatch in passed arguments")
        return kwargs

def strip_end(text, suffix):
    if not text.endswith(suffix):
        return text
    return text[:len(text)-len(suffix)]
