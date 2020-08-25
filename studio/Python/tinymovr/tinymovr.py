
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

        # Now reassig filtered endpoints
        self.endpoints = { key:value for (key,value) in eps.items() if (("from_version" not in value) or
                (parse_version(self.fw_version) >= parse_version(value["from_version"]))) }

    def __getattr__(self, attr):
        if attr in self.endpoints:
            d = self.endpoints[attr]
            if d["type"] == "w":
                def wrapper(*args, **kwargs):
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
                self.iface.send_new(self.node_id, d["ep_id"], rtr=True)
                payload = self.iface.receive(self.node_id, d["ep_id"], 1)
                values = self.codec.deserialize(payload, *d["types"])
                if len(values) == 1:
                    return values[0]
                else:
                    assert("labels" in d)
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

    @property
    def encoder_cpr(self):
        if self._encoder_cpr < 2048:
            self._encoder_cpr = self.motor_info.encoder_cpr
        assert(self._encoder_cpr >= 2048)
        return self._encoder_cpr