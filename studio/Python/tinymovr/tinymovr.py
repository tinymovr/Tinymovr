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

import json
from tinymovr.constants import ControlStates, ControlModes


class Tinymovr:
    def __init__(self, root_node, node_id, iface):
        super().__setattr__(self, "root_node", root_node)
        self.node_id = node_id
        self.iface = iface
        self.root_node.set_getter_cb(self.iface.get_data)
        self.root_node.set_setter_cb(self.iface.set_data)

    def __getattr__(self, __name):
        try:
            return self.root_node.__getattr__(__name)
        except AttributeError:
            return super().__getattr__(__name)

    def __setattr__(self, __name, __value):
        try:
            return self.root_node.__getattr__(__name, __value)
        except AttributeError:
            return super().__setattr__(__name, __value)
