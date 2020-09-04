

.. _command-reference:

Tinymovr Studio Command Reference
#################################

state
*****

| **endpoint**: ``0x03``
| **type**: Read-only

Retrieves the controller state, control mode and error flag.

Arguments
---------

=============  =============  =========  ===========
Argument       Description    Data Type  Data Offset
-------------  -------------  ---------  -----------
``error``      Error Code     uint8      0
``state``      Control State  uint8      1
``mode``       Control Mode   uint8      2
=============  =============  =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.state
    {"error": 0, "state": 0, "mode": 0}


set_state()
***********

| **endpoint**: ``0x07``
| **type**: Write-only

Sets the controller state and control mode.

Arguments
---------

=============  =============  =========  ===========
Argument       Description    Data Type  Data Offset
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

Arguments
---------

=============  ===========  =========  ===========
Argument       Description  Data Type  Data Offset
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
Argument       Description  Data Type  Data Offset
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

Arguments
---------

=============  =================   =========  ===========
Argument       Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``position``   Position Estimate   float32    0
``velocity``   Velocity Estimate   float32    4
=============  =================   =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.encoder_estimates
    {"position": 1000.0, "velocity": 0.0}


setpoints
*********

| **endpoint**: ``0x09``
| **type**: Read-only

Retrieves the position and velocity setpoints of the controller.

Arguments
---------

=============  =================   =========  ===========
Argument       Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``position``   Position Setpoint   float32    0
``velocity``   Velocity Setpoint   float32    4
=============  =================   =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.setpoints
    {"position": 1000.0, "velocity": 0.0}


set_pos_setpoint()
******************

| **endpoint**: ``0x0C``
| **type**: Write-only

Sets the position setpoint, and optionally velocity and current feed-forward values.

Arguments
---------

=============  =================   =========  ===========
Argument       Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``position``   Position Setpoint   float32    0
``velocity``   Velocity Setpoint   int16      4
``current``    Current Setpoint    int16      6
=============  =================   =========  ===========

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

=============  =================   =========  ===========
Argument       Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``velocity``   Velocity Setpoint   float32    0
``current``    Current Setpoint    float32    4
=============  =================   =========  ===========

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

=============  =================   =========  ===========
Argument       Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``current``    Current Setpoint    float32    0
=============  =================   =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.set_cur_setpoint(0.5)


limits
******

| **endpoint**: ``0x15``
| **type**: Read-only

Retrieves the velocity and current limits of the controller.

Arguments
---------

=============  =================   =========  ===========
Argument       Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``velocity``   Velocity Limit      float32    0
``current``    Current  Limit      float32    4
=============  =================   =========  ===========

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

=============  =================   =========  ===========
Argument       Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``velocity``   Velocity Limit      float32    0
``current``    Current  Limit      float32    4
=============  =================   =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.set_limits(velocity=200000.0, current=15.0)


gains
******

| **endpoint**: ``0x18``
| **type**: Read-only

Retrieves the position and velocity gains of the controller.

Arguments
---------

=============  =================   =========  ===========
Argument       Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``position``   Position Gain       float32    0
``velocity``   Velocity Gain       float32    4
=============  =================   =========  ===========

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

=============  =================   =========  ===========
Argument       Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``position``   Position Gain       float32    0
``velocity``   Velocity Gain       float32    4
=============  =================   =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.set_gains(position=25.0, velocity=0.00001)


Iq
***

| **endpoint**: ``0x14``
| **type**: Read-only

Retrieves the current (Iq) setpoint and estimate.

Arguments
---------

=============  =================   =========  ===========
Argument       Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``setpoint``   Iq Setpoint         float32    0
``estimate``   Iq Estimate         float32    4
=============  =================   =========  ===========

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

Arguments
---------

=============  =================   =========  ===========
Argument       Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``A``          A Phase Current     int16      0
``B``          B Phase Current     int16      0
``C``          C Phase Current     int16      0
=============  =================   =========  ===========

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

Arguments
---------

=============  =============  =========  ===========
Argument       Description    Data Type  Data Offset
-------------  -------------  ---------  -----------
``device_id``  Device ID      uint32     0
``fw_major``   FW Major Ver.  uint8      4
``fw_minor``   FW Minor Ver.  uint8      5
``fw_patch``   FW Patch Ver.  uint8      6
``temp``       MCU Temp       uint8      7
=============  =============  =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.device_info
    {"device_id": 99999, "fw_major": 0, "fw_minor": 7, "fw_patch": 1, "temp": 45}


motor_info
***********

| **endpoint**: ``0x1E``
| **type**: Read-only

Retrieves information related to the attached motor.

Arguments
---------

===============   =================  =========  ===========
Argument          Description        Data Type  Data Offset
---------------   -----------------  ---------  -----------
``calibrated``    Calibrated Flag    uint8      0
``R``             Phase Resistance   uint16     1
``pole_pairs``    Motor Pole Pairs   uint8      3
``L``             Phase Inductance   uint16     4
``encoder_cpr``   Encoder CPR        uint16     6
===============   =================  =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.motor_info
    {"calibrated": 1, "R": 0.2, "pole_pairs": 11, "L": 0.00012, "encoder_cpr": 8192}


timings
*******

| **endpoint**: ``0x1B``
| **type**: Read-only

Retrieves MCU timings in each control cycle.

Arguments
---------

===============   =================  =========  ===========
Argument          Description        Data Type  Data Offset
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