***************
Comm Interfaces
***************


CAN Bus
-------

CAN Bus is the primary interface for communicating with Tinymovr. The physical and data link interface adheres to the CAN 2.0 standard. Tinymovr exposes to CAN all communication endpoints defined in firmware.

Data rate
#########

By default, the CAN baud rate is set to 1Mbit/s. This is customizable both through CAN as well as through UART. See :ref:`api-can-rate`. Possible values are 125kbit/s, 250kbit/s, 500kbit/s and 1Mbit/s.

Addressing
##########

Tinymovr uses extended CAN frames for communication. The 29-bit identifier of the extended CAN frame is used for device, endpoint and message sequence identification. The 12 least significant bits of the identifier are reserved for endpoint identification, the next 9 for message sequence identification, and the 8 most significant bits, for device identification. The total number of addressable endpoints in a single device are 4096, the number of addressable devices is 256, and a message sequence can include up to 512 messages. 

Protocol
########

For a detailed description, please see :ref:`integrating` and :ref:`api-reference`.


UART
----

Tinymovr offers UART-based (serial) communication as an alternative to CAN Bus. The UART interface uses the same Avlos binary protocol as CAN, providing **full API parity** with all endpoints available over both interfaces.

.. warning::
   The UART port on Tinymovr is NOT 5V tolerant. Applying 5V voltage will immediately damage the onboard PAC5527 controller. Please use only 3.3V for UART communication.

.. warning::
   The UART port on Tinymovr offers a 3.3v output for driving very light loads (30mA absolute max). Tinymovr cannot be powered by this pin. In addition, most UART adapters offer 5V power, which is incompatible with Tinymovr. **In short: If in doubt, leave this pin disconnected**.

Physical Layer
##############

- **Baud rate**: 115200
- **Data bits**: 8
- **Parity**: None
- **Stop bits**: 1
- **Voltage level**: 3.3V TTL

A standard FTDI-style USB to UART adapter at 3.3V is suitable.

Binary Protocol
###############

UART uses a binary protocol with CRC-16 error detection. The protocol is request-response: Tinymovr only responds to commands initiated by the host.

Frame Format
============

All frames follow this structure:

.. code-block:: text

    | Sync (1) | Length (1) | Hash (1) | EP_ID (2) | CMD (1) | Payload (0-8) | CRC16 (2) |

- **Sync** (1 byte): Frame start marker, always ``0xAA``
- **Length** (1 byte): Number of bytes after Length field, excluding CRC (min 4, max 12)
- **Hash** (1 byte): Low 8 bits of protocol hash (for version verification), or ``0x00`` to skip check
- **EP_ID** (2 bytes): Endpoint ID, little-endian (see :ref:`api-reference` for endpoint list)
- **CMD** (1 byte): Command type: ``0x00`` = read/call, ``0x01`` = write
- **Payload** (0-8 bytes): Data payload (for write commands or function arguments)
- **CRC16** (2 bytes): CRC-16-CCITT checksum, little-endian

CRC Calculation
===============

The CRC-16-CCITT algorithm is used:

- **Polynomial**: 0x1021
- **Initial value**: 0xFFFF
- **Input reflection**: No
- **Output reflection**: No

The CRC is calculated over all bytes from Sync to the end of Payload (excluding the CRC field itself).

Example: Read Bus Voltage
=========================

To read the bus voltage (endpoint ID 4):

**Request frame** (hex):

.. code-block:: text

    AA 04 00 04 00 00 [CRC16]
    |  |  |  |  |  +-- CMD: 0x00 (read)
    |  |  |  +--+-- EP_ID: 0x0004 (Vbus)
    |  |  +-- Hash: 0x00 (skip check)
    |  +-- Length: 4 bytes
    +-- Sync: 0xAA

**Response frame** (hex):

.. code-block:: text

    AA 08 XX 04 00 00 [4-byte float] [CRC16]
    |  |  |  |  |  |  +-- Payload: Vbus value as IEEE 754 float
    |  |  |  +--+--+-- EP_ID + CMD
    |  |  +-- Hash: protocol hash (low 8 bits)
    |  +-- Length: 8 bytes
    +-- Sync: 0xAA

Example: Set Position Setpoint
==============================

To set position setpoint (endpoint ID 25) to 1000.0 ticks:

**Request frame** (hex):

.. code-block:: text

    AA 08 00 19 00 01 00 00 7A 44 [CRC16]
    |  |  |  |  |  |  +----------+-- Payload: 1000.0f (IEEE 754: 0x447A0000)
    |  |  |  |  |  +-- CMD: 0x01 (write)
    |  |  |  +--+-- EP_ID: 0x0019 (25 = controller.position.setpoint)
    |  |  +-- Hash: 0x00
    |  +-- Length: 8 bytes
    +-- Sync: 0xAA

Write commands do not return a response unless the endpoint is a function with a return value.

API Reference
#############

UART provides access to all Avlos endpoints. See :ref:`api-reference` for the complete endpoint list with IDs, data types, and descriptions.

Common endpoints:

- **ID 0**: ``protocol_hash`` - Protocol version hash (uint32)
- **ID 4**: ``Vbus`` - Bus voltage in volts (float)
- **ID 21**: ``controller.state`` - Controller state (uint8)
- **ID 25**: ``controller.position.setpoint`` - Position setpoint in ticks (float)
- **ID 42**: ``controller.calibrate()`` - Start calibration (void)
- **ID 43**: ``controller.idle()`` - Set idle mode (void)
