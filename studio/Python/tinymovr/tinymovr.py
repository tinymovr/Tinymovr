""" Tinymovr base module.

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
"""

import time
from copy import copy
import pkg_resources
from packaging import version
import json
from tinymovr.iface import IFace
from tinymovr.presenter import presenter_map, strip_end
from tinymovr.constants import ControlStates, ControlModes
from pint import Quantity as _Q


min_fw_version = "0.8.16"


class VersionError(Exception):
    def __init__(self, kw, found, required, *args, **kwargs):
        msg = "Node {} version incompatible (found: {}, required: {})".format(
            kw, found, required
        )
        super().__init__(msg, *args, **kwargs)
        self.kw = kw
        self.found = found
        self.required = required


class Tinymovr:
    def __init__(self, node_id: int, iface: IFace, version_check=True):
        self.node_id: int = node_id
        self.iface: IFace = iface
        self.eps = self.iface.get_ep_map()
        self.codec = self.iface.get_codec()

        di = self.device_info
        self.fw_version = ".".join(
            [str(di.fw_major), str(di.fw_minor), str(di.fw_patch)]
        )
        if version_check:
            # Check FW version
            if version.parse(self.fw_version) < version.parse(min_fw_version):
                raise VersionError(
                    kw="fw", found=self.fw_version, required=min_fw_version
                )
            # Check studio version
            msv = self.min_studio_version
            msv_str = ".".join(
                [str(msv.fw_major), str(msv.fw_minor), str(msv.fw_patch)]
            )
            studio_version_string = pkg_resources.require("tinymovr")[0].version
            if version.parse(studio_version_string) < version.parse(msv_str):
                raise VersionError(
                    kw="studio", found=studio_version_string, required=msv_str
                )

    def __getattr__(self, attr: str):

        if attr in self.eps:
            d = self.eps[attr]
            ep_type = d["type"]

            if "w" in ep_type:
                # This is a write or read-write endpoint
                def wrapper(*args, **kwargs):
                    assert (
                        len(args) == 0 or len(kwargs) == 0
                    ), "Either positional or keyword arguments are supported, not both"
                    if len(kwargs) > 0:
                        assert "labels" in d
                        inputs = [
                            kwargs[k] if k in kwargs else d["defaults"][k]
                            for k in d["labels"]
                        ]
                    elif len(args) > 0:
                        inputs = [
                            args[i] if i < len(args) else d["defaults"][k]
                            for i, k in enumerate(d["labels"])
                        ]
                    else:
                        inputs = []
                    if "units" in d:
                        inputs = [
                            v.to(d["units"][i]).magnitude if isinstance(v, _Q) else v
                            for i, v in enumerate(inputs)
                        ]
                    payload = None
                    if len(inputs) > 0:
                        payload = self.codec.serialize(inputs, *d["types"])
                    self.iface.send(self.node_id, d["ep_id"], payload=payload)
                    if "r" in ep_type:
                        return self.present_response(
                            attr, d, self.iface.receive(self.node_id, d["ep_id"])
                        )

                return wrapper

            elif ep_type == "r":
                # This is a read-type endpoint
                self.iface.send(self.node_id, d["ep_id"])
                return self.present_response(
                    attr, d, self.iface.receive(self.node_id, d["ep_id"])
                )

    def present_response(self, attr, ep, response):
        data = self.codec.deserialize(response, *ep["types"])
        if attr in presenter_map:
            return presenter_map[attr](attr, data, ep)
        return presenter_map["default"](attr, data, ep)

    def calibrate(self):
        self.set_state(ControlStates.Calibration)

    def idle(self):
        self.set_state(ControlStates.Idle)

    def position_control(self):
        self.set_state(ControlStates.ClosedLoopControl, ControlModes.PositionControl)

    def velocity_control(self):
        self.set_state(ControlStates.ClosedLoopControl, ControlModes.VelocityControl)

    def current_control(self):
        self.set_state(ControlStates.ClosedLoopControl, ControlModes.CurrentControl)

    def export_config(self, file_path: str):
        """
        Export the board config to a file
        """
        config_map = {}
        for ep_name, ep in self.iface.get_ep_map().items():
            if ep["type"] == "r" and "ser_map" in ep:
                # Node can be serialized (saved)
                print("Exporting {}...".format(ep_name))
                config_map.update(
                    self._data_from_arguments(getattr(self, ep_name), ep["ser_map"])
                )
        with open(file_path, "w") as f:
            json.dump(config_map, f)

    def restore_config(self, file_path: str):
        """
        Restore the board config from a file
        """
        with open(file_path, "r") as f:
            data = json.load(f)
        for ep_name, ep in self.iface.get_ep_map().items():
            if ep["type"] == "w" and "ser_map" in ep:
                # Node has saved data and can be deserialized (restored)
                kwargs = self._arguments_from_data(ep["ser_map"], data)
                if len(kwargs):
                    f = getattr(self, ep_name)
                    print("Restoring {}...".format(ep_name))
                    f(**kwargs)
                    time.sleep(0.001) #safer to sleep cause setting is async

    # "ser_map": {"can": ("id", "baud_rate")}
    def _data_from_arguments(self, values, ep_map):
        """
        Generate a nested dictionary from a dictionary of values,
        following the template in ep_map
        """
        data = {}
        for key, value in ep_map.items():
            if isinstance(value, dict):
                data[key] = self._data_from_arguments(values, value)
            elif isinstance(value, tuple):
                content = {}
                for k in value:
                    try:
                        content[k] = getattr(values, k).magnitude
                    except AttributeError:
                        content[k] = getattr(values, k)
                data[key] = content
            else:
                raise TypeError("Map is not a dictionary or tuple")
        return data

    def _arguments_from_data(self, ep_map, ep_data):
        """
        Generate a flat argument dictionary from a nested dictionary
        containing values for keys in endpoint labels
        """
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

    def __dir__(self):
        eps = list(self.iface.get_ep_map().keys())
        blacklist = ["iface", "node_id", "fw_version"]
        self_attrs = [
            k
            for k in object.__dir__(self)
            if not k.startswith("_") and k not in blacklist
        ]
        self_attrs
        return eps + self_attrs
