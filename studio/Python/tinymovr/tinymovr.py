
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

import copy
import numbers
import json
from pkg_resources import parse_version
from tinymovr.iface import CAN, CANBusCodec, DataType
from tinymovr import Endpoints
from tinymovr.attr_object import AttrObject

class Tinymovr:

    def __init__(self, node_id, iface, codec=CANBusCodec(), eps=Endpoints):
        self.node_id = node_id
        self.iface = iface
        self.codec = codec
        self._encoder_cpr = -1

        # Temporarily assign to self.endpoints purely for convenience
        self.endpoints = eps
        di = self.device_info
        self.fw_version = '.'.join([str(di.fw_major), str(di.fw_minor), str(di.fw_patch)])

        # Now reassign filtered endpoints
        self.endpoints = { key:value for (key,value) in eps.items() if (("from_version" not in value) or
                (parse_version(self.fw_version) >= parse_version(value["from_version"]))) }

    def __getattr__(self, attr):
        if attr in self.endpoints:
            d = self.endpoints[attr]
            if d["type"] == "w":
                # This is a write-type endpoint
                def wrapper(*args, **kwargs):
                    assert(len(args) == 0 or len(kwargs) == 0)
                    if len(kwargs) > 0:
                        assert("labels" in d)
                        f_args = [kwargs[k] for k in d["labels"]]
                    else:
                        f_args = copy.copy(args)
                    if "types" in d:
                        slack = len(d["types"]) - len(f_args)
                        if slack > 0:
                            slack_defaults = d["defaults"][-slack:]
                            f_args = f_args + slack_defaults
                        payload = self.codec.serialize(f_args, *d["types"])
                        self.iface.send_new(self.node_id, d["ep_id"], payload=payload)
                    else:
                        self.iface.send_new(self.node_id, d["ep_id"])
                return wrapper
            elif d["type"] == "r":
                # This is a read-type endpoint
                self.iface.send_new(self.node_id, d["ep_id"], rtr=True)
                payload = self.iface.receive(self.node_id, d["ep_id"])
                values = self.codec.deserialize(payload, *d["types"])
                if len(values) == 1:
                    return values[0]
                else:
                    return AttrObject(d["labels"], values)

    def __dir__(self):
        return list(self.endpoints.keys())

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

    def export_config(self, file_path):
        '''
        Export the board config to a file
        '''
        config_map = {}
        for ep_id in self.endpoints:
            ep = self.endpoints[ep_id]
            if ep["type"] == 'r' and "ser_map" in ep:
                # Node can be serialized (saved)
                vals = getattr(self, ep_id)
                config_map.update(self._data_from_arguments(vals, ep["ser_map"]))
        with open(file_path, 'w') as f:
            json.dump(config_map, f)

    def restore_config(self, file_path):
        '''
        Restore the board config from a file
        '''
        with open(file_path, 'r') as f:
            data = json.load(f)
        for ep_id in self.endpoints:
            ep = self.endpoints[ep_id]
            if ep["type"] == 'w' and "ser_map" in ep:
                # Node has saved data and can be deserialized (restored)
                kwargs = self._arguments_from_data(ep["ser_map"], data)
                if len(kwargs):
                    f = getattr(self, ep_id)
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
                    kwargs.update(self._arguments_from_data(value, ep_data[key]))
        elif isinstance(ep_map, tuple) and isinstance(ep_data, dict):
            for key in ep_map:
                if key in ep_data:
                    kwargs[key] = ep_data[key]
        else:
            raise TypeError("Mismatch in passed arguments")
        return kwargs

    @property
    def encoder_cpr(self):
        if self._encoder_cpr < 2048:
            self._encoder_cpr = self.motor_info.encoder_cpr
        assert(self._encoder_cpr >= 2048)
        return self._encoder_cpr