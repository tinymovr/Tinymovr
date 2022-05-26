"""
Tinymovr Units Module
Copyright Ioannis Chatzikonstantinou 2020-2022

Defines the units registry used by Studio

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

import pint

_registry = None


def get_registry():
    global _registry
    if not _registry:
        _registry = pint.UnitRegistry()
        _registry.define("tick = turn / 8192")
    return _registry
