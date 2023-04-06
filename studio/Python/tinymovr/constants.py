"""
Tinymovr Constants
Copyright Ioannis Chatzikonstantinou 2020-2023

Defines various constants used throughout Tinymovr Studio

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

import math
from enum import IntEnum


app_name = "Tinymovr Studio"
base_node_name = "tm"
HEARTBEAT_BASE = 0x700

CAN_EP_SIZE = 12
CAN_EP_MASK = (1 << CAN_EP_SIZE) - 1
CAN_SEQ_SIZE = 9
CAN_SEQ_MASK = ((1 << CAN_SEQ_SIZE) - 1) << CAN_EP_SIZE
CAN_DEV_SIZE = 8
CAN_DEV_MASK = ((1 << CAN_DEV_SIZE) - 1) << (CAN_EP_SIZE + CAN_SEQ_SIZE)

