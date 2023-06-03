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

As an alternative to CAN Bus, Tinymovr offers UART-based (serial) communication. The protocol is much simpler than CAN and mainly designed for troubleshooting or testing in the absence of CAN hardware.

.. warning::
   The UART port on Tinymovr is NOT 5V tolerant. Applying 5V voltage will immediately damage the onboard PAC5527 controller. Please use only 3.3V for UART communication.

.. warning::
   The UART port on Tinymovr offers a 3.3v output for driving very light loads (30mA absolute max). Tinymovr cannot be powered by this pin. In addition, most UART adapters offer 5V power, which is incompatible with Tinymovr. **In short: If in doubt, leave this pin disconnected**.

Protocol Description
####################

The UART port is TTL at 115200 baud. A regular FTDI-style USB to UART adapter should be sufficient.

UART communication is based on a simple human-readable protocole dubbed the "dot protocol", because the dot is the command starting character. The protocol is response-only, meaning that Tinymovr will only respond to commands initiated by the client, it will never initiate a transmission on it's own.

The command template is as follows:

.. code-block:: shell

    .Cxxxx

The command begins with a dot. The next single character identifies the command. The characters following the second one are optional values to pass to write commands. Read commands only include the dot and command character. The command is finalized with a newline character (\n, not shown above).

For instance, to get the current position estimate:

.. code-block:: shell

    command: .p
    response: 1000

To set the velocity estimate in encoder ticks:

.. code-block:: shell

    command: .V10000
    (no response)

The values passed or returned are always integers scaled by the appropriate factor (see command reference below).

Note that command characters are case-sensitive, i.e. capitals and small represent different commands. As a convention, capital letters are setters and small are getters, where applicable.

Command Reference
#################

.Z
==

Transition to idle state.

Example

.. code-block:: shell

    .Z
    0

.Q
==

Transition to calibration state.

Example

.. code-block:: shell

    .Q
    0

.A
==

Transition to close loop control state.

Example

.. code-block:: shell

    .A
    0

.e
==

Get the error code.

Example

.. code-block:: shell

    .e
    0

.p
==

Get position estimate (ticks).

Example

.. code-block:: shell

    .p
    1000

.v
==

Get velocity estimate (ticks/s).


Example

.. code-block:: shell

    .v
    -200

.i
==

Get current (Iq) estimate (mA).

Example

.. code-block:: shell

    .i
    2000

.P
==

Get/set position setpoint (ticks).

Example

.. code-block:: shell

    .P
    1000

.. code-block:: shell

    .P1000

.V
==

Get/set velocity setpoint (ticks/s).

Example

.. code-block:: shell

    .V
    -10000

.. code-block:: shell

    .V-10000

.I
==

Get/set current (Iq) setpoint (mA).

Example

.. code-block:: shell

    .I
    1000

.. code-block:: shell

    .I1000

.W
==

Get/set current (Iq) limit (mA).

Example

.. code-block:: shell

    .W
    10000

.. code-block:: shell

    .W15000
    
.Y
==

Get/set position gain.

Example

.. code-block:: shell

    .Y
    25

.. code-block:: shell

    .Y25
    
.F
==

Get/set velocity gain (x0.000001).

Example

.. code-block:: shell

    .F
    20

.. code-block:: shell

    .F20

.G
==

Get/set velocity integrator gain (x0.001).

Note that high values (e.g. above 10) may cause instability.

Example

.. code-block:: shell

    .G
    2

.. code-block:: shell

    .G2

.h
==

Get motor phase resistance (mOhm).

Example

.. code-block:: shell

    .h
    200

.l
==

Get motor phase inductance (μH).

Example

.. code-block:: shell

    .l
    2000

.R
==

Reset the MCU.

Example

.. code-block:: shell

    .R

.S
==

Save board configuration.

Example

.. code-block:: shell

    .S

.X
==

Erase board configuration and reset.

Example

.. code-block:: shell

    .X
