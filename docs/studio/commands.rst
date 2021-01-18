
.. _command-reference:

Studio Command Reference
########################

state
*****

| **endpoint**: ``0x03``
| **type**: Read-only

Retrieves an object containing the controller state, control mode and error flags.
The object is pretty-printed if inside the Tinymovr Studio iPython environment.

This command has been revised as of firmware 0.8.2 and studio 0.3.3 to report multiple error flags if available. The above and newer versions can display up to five error flags simultaneously, and with the order that they were registered by the firmware error handler.

Tinymovr Studio 0.3.3 and newer is backwards compatible with the legacy error reporting system, as such newer Studio versions can be used with older firmware. However, newer firmware (0.8.2 and later) is not compatible with older Studio versions. Make sure that you run the latest version of Studio before upgrading your firmware.

Return Values
-------------

=============  ==================  =========  ===========
Member         Description         Data Type  Data Offset
-------------  ------------------  ---------  -----------
``errors``     Legacy Error Flag   uint8      0
``state``      Control State       uint8      1
``mode``       Control Mode        uint8      2
``errors``     1st Error Flag      uint8      3
``...``        2nd Error Flag      uint8      4
``...``        3rd Error Flag      uint8      5
``...``        4th Error Flag      uint8      6
``...``        5th Error Flag      uint8      7
=============  ==================  =========  ===========

Example
-------

Legacy system

.. code-block:: python

    >>>tmx.state
    {"error": 0, "state": 0, "mode": 0}

New system

.. code-block:: python

    >>>tmx.state
    State: Idle     Mode: Position
    Errors:
        Invalid State (1): Attempt to transition to invalid state

    >>>tmx.state.mode
    0


set_state()
***********

| **endpoint**: ``0x07``
| **type**: Write-only

Sets the controller state and control mode.

Arguments
---------

=============  =============  =========  ===========
Member         Description    Data Type  Data Offset
-------------  -------------  ---------  -----------
``state``      Control State  uint8      1
``mode``       Control Mode   uint8      2
=============  =============  =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.set_state(state=0, mode=0)


can_config
**********

| **endpoint**: ``0x05``
| **type**: Read-only

Retrieves the CAN configuration.

Return Values
-------------

=============  ===========  =========  ===========
Member         Description  Data Type  Data Offset
-------------  -----------  ---------  -----------
``id``         CAN Bus ID   uint8      0
``baud_rate``  Baud Rate    uint16     1
=============  ===========  =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.can_config
    {"id": 1, "baud_rate": 250}


set_can_config()
****************

| **endpoint**: ``0x06``
| **type**: Write-only

Sets the CAN configuration.

Arguments
---------

=============  ===========  =========  ===========
Member         Description  Data Type  Data Offset
-------------  -----------  ---------  -----------
``id``         CAN Bus ID   uint8      0
``baud_rate``  Baud Rate    uint16     1
=============  ===========  =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.set_can_config(id=1, baud_rate=250)


encoder_estimates
*****************

| **endpoint**: ``0x09``
| **type**: Read-only

Retrieves the position and velocity encoder estimates.

Return Values
-------------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``position``   Position Estimate   float32    0           ticks
``velocity``   Velocity Estimate   float32    4           ticks/second
=============  =================   =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.encoder_estimates
    {"position": 1000.0, "velocity": 0.0}


setpoints
*********

| **endpoint**: ``0x0A``
| **type**: Read-only

Retrieves the position and velocity setpoints of the controller.

Return Values
-------------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``position``   Position Setpoint   float32    0           tick
``velocity``   Velocity Setpoint   float32    4           tick/second
=============  =================   =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.setpoints
    {"position": 1000.0, "velocity": 0.0}


set_pos_setpoint()
******************

| **endpoint**: ``0x0C``
| **type**: Write-only

Sets the position setpoint, and optionally velocity and current feed-forward values. Due to the fact that data types of feed-forward values are range-limited, multiples of the root units are used.

Arguments
---------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``position``   Position Setpoint   float32    0           tick
``velocity``   Velocity Setpoint   int16      4           decatick/second
``current``    Current Setpoint    int16      6           centiampere
=============  =================   =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.set_pos_setpoint(1000.0)

.. code-block:: python

    >>>tmx.set_pos_setpoint(position=1000.0, velocity=10000.0, current=0.0)


set_vel_setpoint()
******************

| **endpoint**: ``0x0D``
| **type**: Write-only

Sets the velocity setpoint, and optionally current feed-forward value.

Arguments
---------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``velocity``   Velocity Setpoint   float32    0           ticks/second
``current``    Current Setpoint    float32    4           ampere
=============  =================   =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.set_vel_setpoint(10000.0)

.. code-block:: python

    >>>tmx.set_vel_setpoint(velocity=10000.0, current=0.0)


set_cur_setpoint()
******************

| **endpoint**: ``0x0E``
| **type**: Write-only

Sets the current (Iq) setpoint.

Arguments
---------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``current``    Current Setpoint    float32    0           amperes
=============  =================   =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.set_cur_setpoint(0.5)


limits
******

| **endpoint**: ``0x15``
| **type**: Read-only

Retrieves the velocity and current limits of the controller.

Return Values
-------------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``velocity``   Velocity Limit      float32    0           tick/second
``current``    Current  Limit      float32    4           ampere
=============  =================   =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.limits
    {"velocity": 300000.0, "current": 10.0}


set_limits()
************

| **endpoint**: ``0x0F``
| **type**: Write-only

Sets the velocity and current limits of the controller.

Arguments
---------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``velocity``   Velocity Limit      float32    0           tick/second
``current``    Current  Limit      float32    4           ampere
=============  =================   =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.set_limits(velocity=200000.0, current=15.0)


gains
******

| **endpoint**: ``0x18``
| **type**: Read-only

Retrieves the position and velocity gains of the controller.

Return Values
-------------

=============  =================   =========  =========== ==================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ------------------
``position``   Position Gain       float32    0           1/second
``velocity``   Velocity Gain       float32    4           ampere*second/tick
=============  =================   =========  =========== ==================

Example
-------

.. code-block:: python

    >>>tmx.gains
    {"position": 35.0, "velocity": 0.000012}


set_gains()
************

| **endpoint**: ``0x19``
| **type**: Write-only

Sets the position and velocity gains of the controller.

Arguments
---------

=============  =================   =========  =========== ==================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ------------------
``position``   Position Gain       float32    0           1/second
``velocity``   Velocity Gain       float32    4           ampere*second/tick
=============  =================   =========  =========== ==================

Example
-------

.. code-block:: python

    >>>tmx.set_gains(position=25.0, velocity=0.00001)


integrator_gains
****************

| **endpoint**: ``0x18``
| **type**: Read-only

Retrieves the velocity integrator gain of the controller.

Return Values
-------------

=============  =========================   =========  =========== ==================
Member         Description                 Data Type  Data Offset Default Unit
-------------  -------------------------   ---------  ----------- ------------------
``velocity``   Velocity Integrator Gain    float32    0           ampere*second/tick
=============  =========================   =========  =========== ==================

Example
-------

.. code-block:: python

    >>>tmx.integrator_gains
    {"velocity": 0.0001}


set_integrator_gains()
**********************

| **endpoint**: ``0x19``
| **type**: Write-only

Sets the velocity integrator gain of the controller.

Arguments
---------

=============  =========================   =========  =========== ==================
Member         Description                 Data Type  Data Offset Default Unit
-------------  -------------------------   ---------  ----------- ------------------
``velocity``   Velocity Integrator Gain    float32    0           ampere*second/tick
=============  =========================   =========  =========== ==================

Example
-------

.. code-block:: python

    >>>tmx.set_integrator_gains(velocity=0.0001)


Iq
***

| **endpoint**: ``0x14``
| **type**: Read-only

Retrieves the current (Iq) setpoint and estimate.

Return Values
-------------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``setpoint``   Iq Setpoint         float32    0           ampere
``estimate``   Iq Estimate         float32    4           ampere
=============  =================   =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.Iq
    {"setpoint": 1.0, "estimate": 0.9}


Iphase
******

| **endpoint**: ``0x10``
| **type**: Read-only

Retrieves the measured phase currents.

Return Values
-------------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``A``          A Phase Current     int16      0           ampere
``B``          B Phase Current     int16      0           ampere
``C``          C Phase Current     int16      0           ampere
=============  =================   =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.Iphase
    {"A": 1.0, "B": -0.6, "C": -0.4}


device_info
***********

| **endpoint**: ``0x1A``
| **type**: Read-only

Retrieves device-related information.

Return Values
-------------

=============  =============  =========  =========== ================
Member         Description    Data Type  Data Offset Default Unit
-------------  -------------  ---------  ----------- ----------------
``device_id``  Device ID      uint32     0
``fw_major``   FW Major Ver.  uint8      4
``fw_minor``   FW Minor Ver.  uint8      5
``fw_patch``   FW Patch Ver.  uint8      6
``temp``       MCU Temp       uint8      7           °C
=============  =============  =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.device_info
    {"device_id": 99999, "fw_major": 0, "fw_minor": 7, "fw_patch": 1, "temp": 45}


motor_config
***********

| **endpoint**: ``0x1E``
| **type**: Read-only

Retrieves information related to the attached motor.

Return Values
-------------

===============   =================  =========  =========== ================
Member            Description        Data Type  Data Offset Default Unit
---------------   -----------------  ---------  ----------- ----------------
``calibrated``    Calibrated Flag    uint8      0
``R``             Phase Resistance   uint16     1           milliohm
``pole_pairs``    Motor Pole Pairs   uint8      3
``L``             Phase Inductance   uint16     4           microhenry
===============   =================  =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.motor_config
    {"calibrated": 1, "R": 0.2, "pole_pairs": 11, "L": 0.00012, "encoder_ticks": 8192}


timings
*******

| **endpoint**: ``0x1B``
| **type**: Read-only

Retrieves MCU timings in each control cycle.

Return Values
-------------

===============   =================  =========  ===========
Member            Description        Data Type  Data Offset
---------------   -----------------  ---------  -----------
``total``         Total MCU Cycles   uint32     0
``busy``          Busy MCU Cycles    uint32     4
===============   =================  =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.timings
    {"total": 7500, "busy": 1000}


estop()
*******

| **endpoint**: ``0x02``
| **type**: Write-only

Emergency stop: Idles the MCU immediately.

Arguments
---------

No arguments.

Example
-------

.. code-block:: python

    >>>tmx.estop()


reset()
*******

| **endpoint**: ``0x16``
| **type**: Write-only

Resets the MCU.

Arguments
---------

No arguments.

Example
-------

.. code-block:: python

    >>>tmx.reset()


save_config()
*************

| **endpoint**: ``0x1C``
| **type**: Write-only

Saves board configuration to Non-Volatile Memory.

Arguments
---------

No arguments.

Example
-------

.. code-block:: python

    >>>tmx.save_config()


erase_config()
**************

| **endpoint**: ``0x1D``
| **type**: Write-only

Erases board configuration and resets the MCU.

Arguments
---------

No arguments.

Example
-------

.. code-block:: python

    >>>tmx.erase_config()
