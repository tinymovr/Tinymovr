API
+++

Overview
########

This document outlines the main API used to interface with Tinymovr. This API comprises a series of read/write endpoints. The endpoints are defined taking into account the capabilities and constraints of the CAN bus, the main communication bus used by Tinymovr. 

The Tinymovr API allows full hardware control from within Python scripts, using a high-level interface to hardware. At the same time, it is possible to interface directly with the CAN bus endpoints, for instance in an embedded application. In both cases, the :ref:`api-reference` provides all the necessary information.

Tinymovr API is part of Tinymovr Studio. For help installing Studio, please take a look at :ref:`studio-installation`.

Use with Python
###############

Here below is an example using the API from Python scripts and controlling hardware:

.. code-block:: python

    import can
    from tinymovr import Tinymovr
    from tinymovr.iface.can import CAN

    bus = can.Bus(bustype="cantact", channel="COM1", bitrate=1000000)
    iface = CAN(bus)
    tm = Tinymovr(node_id=1, iface=iface)

    tm.calibrate()

The above code block will instantiate a Tinymovr with CAN bus id of 1 and calibrate it. Following the above, you can issue commands such as:

.. code-block:: python

    tm.position_control()
    tm.set_pos_setpoint(0)

    tm.velocity_control()
    tm.set_vel_setpoint(80000)

.. _api-reference:

API Reference
#############

.. note::
    Where "float32" is mentioned, an IEEE 754, 32-bit floating point representation is assumed.

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

.. note::
    Results of calibration are not automatically saved to Non-Volatile Memory (NVM). You need to issue a ``save_config`` command after calibration is finished to save calibration data to NVM.

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

.. _api-can-config:

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

Sets the position setpoint, and optionally velocity and current feed-forward values. Due to the fact that data types of feed-forward values are limited by type, multiples of the root units are used.

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


offset_dir
**********

| **endpoint**: ``0x02``
| **type**: Read-only

Retrieves the user defined rotor position offset and rotor direction values.

Return Values
-------------

=============  =================   =========  =========== ==================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ------------------
``offset``     Offset              float32    0           tick
``direction``  Direction           int8       4           
=============  =================   =========  =========== ==================

Example
-------

.. code-block:: python

    >>>tmx.offset_dir
    {"offset": 0.0, "direction": 1}


set_offset_dir()
****************

| **endpoint**: ``0x08``
| **type**: Write-only

Sets the user defined rotor position offset and rotor direction values.

.. note::
    The ``direction`` field only accepts -1 or 1 as values. All other values are ignored.


Arguments
---------

=============  =================   =========  =========== ==================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ------------------
``offset``     Offset              float32    0           tick
``direction``  Direction           int8       4           
=============  =================   =========  =========== ==================

Example
-------

.. code-block:: python

    >>>tmx.set_gains(offset=2500, direction=-1)


.. _integrator-deadband:

vel_integrator_params
*********************

| **endpoint**: ``0x18``
| **type**: Read-only

Retrieves the velocity integrator gain and deadband parameters.

Return Values
-------------

=============  ============================   =========  =========== ==================
Member         Description                    Data Type  Data Offset Default Unit
-------------  ----------------------------   ---------  ----------- ------------------
``gain``       Velocity Integrator Gain       float32    0           ampere*second/tick
``deadband``   Velocity Integrator Deadband   float32    4           tick
=============  ============================   =========  =========== ==================

Example
-------

.. code-block:: python

    >>>tmx.vel_integrator_params
    {"gain": 0.0001, deadband: 200}


set_vel_integrator_params()
***************************

| **endpoint**: ``0x19``
| **type**: Write-only

Sets the velocity integrator gain and deadband parameters.

Arguments
---------

=============  ============================   =========  =========== ==================
Member         Description                    Data Type  Data Offset Default Unit
-------------  ----------------------------   ---------  ----------- ------------------
``gain``       Velocity Integrator Gain       float32    0           ampere*second/tick
``deadband``   Velocity Integrator Deadband   float32    4           tick
=============  ============================   =========  =========== ==================

Example
-------

.. code-block:: python

    >>>tmx.set_vel_integrator_params(gain=0.0001, deadband=300)


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


plan_t_limit
************

| **endpoint**: ``0x20``
| **type**: Write-only

Generate and execute a time-limited trajectory.

Arguments
-------------

===================  ==========================  =========  ===========  ====================
Member               Description                 Data Type  Data Offset  Default Unit
-------------------  --------------------------  ---------  -----------  --------------------
``target_position``  Target Position             float      0            tick
``total_time``       Total Trajectory Time       uint16     4            millisecond
``acc_percent``      Acceleration Phase Percent  uint8      6            (none, values 0-255)
``dec_percent``      Deceleration Phase Percent  uint8      7            (none, values 0-255)
===================  ==========================  =========  ===========  ====================

Example
-------

.. code-block:: python

    >>>tmx.plan_t_limit(100000, 3000, 50, 50)


plan_v_limit
************

| **endpoint**: ``0x21``
| **type**: Write-only

Generate and execute an acceleration- and velocity-limited trajectory.

Arguments
-------------

===================  ==========================  =========  ===========  ================
Member               Description                 Data Type  Data Offset  Default Unit
-------------------  --------------------------  ---------  -----------  ----------------
``target_position``  Target Position             float      0            tick
``max_velocity``     Max Velocity                float      4            tick/second
===================  ==========================  =========  ===========  ================

Example
-------

.. code-block:: python

    >>>tmx.plan_v_limit(100000, 50000)


set_max_plan_acc_dec
********************

| **endpoint**: ``0x22``
| **type**: Write-only

Set maximum acceleration and deceleration values for trajectory generation.

.. note::
    This command only sets values, it does not execute a trajectory. For trajecotry execution with set values, make a call to ``plan_v_limit``.

Arguments
-------------

===================  ==========================  =========  ===========  ================
Member               Description                 Data Type  Data Offset  Default Unit
-------------------  --------------------------  ---------  -----------  ----------------
``max_acc``          Max Acceleration            float      0            tick/(second^2)
``max_dec``          Max Deceleration            float      4            tick/(second^2)
===================  ==========================  =========  ===========  ================

Example
-------

.. code-block:: python

    >>>tmx.set_max_plan_acc_dec(50000, 50000)


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
************

| **endpoint**: ``0x1E``
| **type**: Read-only

Retrieves motor config (flags, pole pairs, calibration current).

Return Values
-------------

===============   ===================  =========  =========== ================
Member            Description          Data Type  Data Offset Default Unit
---------------   -------------------  ---------  ----------- ----------------
``flags``         Calibrated, Gimbal   uint8      0
``R``             Phase Resistance     uint16     1           milliohm
``pole_pairs``    Motor Pole Pairs     uint8      3
``L``             Phase Inductance     uint16     4           microhenry
``I_cal``         Calibration Current  uint16     6           milliamp
===============   ===================  =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.motor_config
    {"flags": 1, "R": 200, "pole_pairs": 11, "L": 100, "I_cal": 5000}


set_motor_config
****************

| **endpoint**: ``0x1F``
| **type**: Write-only

Sets motor properties.

Arguments
---------

===============   ===================  =========  =========== ================
Member            Description          Data Type  Data Offset Default Unit
---------------   -------------------  ---------  ----------- ----------------
``flags``         Gimbal               uint8      0
``R``             Phase Resistance     uint16     1           milliohm
``L``             Phase Inductance     uint16     3           microhenry
``I_cal``         Calibration Current  uint16     5           milliamp
===============   ===================  =========  =========== ================

Example
-------

High-current motor:
.. code-block:: python

    >>>tmx.set_motor_config(0, 200, 100, 5000)

Gimbal motor:
.. code-block:: python

    >>>tmx.set_motor_config(1, 10000, 2000, 500)

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

.. note::
    Saving config only works when Tinymovr is in idle mode, otherwise the command is ignored.

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

Erases the configuration stored in NVM and resets the MCU.

.. note::
    Erasing config only works when Tinymovr is in idle mode, otherwise the command is ignored.

Arguments
---------

No arguments.

Example
-------

.. code-block:: python

    >>>tmx.erase_config()


get_set_pos_vel()
******************
    
| **endpoint**: ``0x25``
| **type**: Read-Write

Gets and sets Position and Velocity feedforward in one go.

Arguments
---------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``position``   Position Setpoint   float32    0           ticks
``velocity``   Velocity Setpoint   float32    4           ticks/second
=============  =================   =========  =========== ================

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

    >>>tmx.get_set_pos_vel(1000.0, 0)
    {"position":0.0, "velocity": 0.0}


get_set_pos_vel_Iq()
********************
    
| **endpoint**: ``0x26``
| **type**: Read-Write

Get and set Position, Velocity feedforward and Iq feedforward in one go. Due to the fact that data types of feed-forward values are limited by type, multiples of the root units are used.

Arguments
---------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``position``   Position Setpoint   float32    0           tick
``velocity``   Velocity Setpoint   int16      4           decatick/second
``current``    Current Setpoint    int16      6           centiampere
=============  =================   =========  =========== ================

Return Values
-------------

=============  =================   =========  =========== ================
Member         Description         Data Type  Data Offset Default Unit
-------------  -----------------   ---------  ----------- ----------------
``position``   Position Estimate   float32    0           tick
``velocity``   Velocity Estimate   int16      4           decatick/second
``current``    Current Estimate    int16      6           centiampere
=============  =================   =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.get_set_pos_vel_Iq(1000.0, 0, 0)
    {"position":0.0, "velocity": 0.0, "current": 0.0}


motor_RL
********

| **endpoint**: ``0x27``
| **type**: Read-only

Retrieves motor resistance and inductance values.

Return Values
-------------

===============   ===================  =========  =========== ================
Member            Description          Data Type  Data Offset Default Unit
---------------   -------------------  ---------  ----------- ----------------
``R``             Phase Resistance     float32    0           ohm
``L``             Phase Inductance     float32    4           henry
===============   ===================  =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.motor_RL
    {"R": 0.2, "L": 0.00005}


set_motor_RL
************

| **endpoint**: ``0x28``
| **type**: Write-only

Sets attached motor resistance and inductance values.

Arguments
---------

===============   ===================  =========  =========== ================
Member            Description          Data Type  Data Offset Default Unit
---------------   -------------------  ---------  ----------- ----------------
``R``             Phase Resistance     float32    0           ohm
``L``             Phase Inductance     float32    4           henry
===============   ===================  =========  =========== ================

Example
-------

.. code-block:: python

    >>>tmx.set_motor_RL(0.5, 0.0001)


Error Codes
###########

Tinymovr uses error codes to indicate faults in operation. These are listed below. Note that using Tinymovr studio, the error codes are already presented with an explanation.

0: ``NO_ERROR``
***************

No error present.

1: ``INVALID_STATE``
********************

An invalid state has been requested. This can be triggered when attempting to transition to a state whose controller state constraints are not satisfied. E.g. switching to closed loop control without calibrating.

2: ``ILLEGAL_VALUE``
********************

This is a legacy error code that is not in use.

3: ``VBUS_UNDERVOLTAGE``
************************

The bus voltage has dropped below the undervoltage threshold. In a current-limited power supply, this may also indicate excessive current demand from the power supply.

4: ``OVERCURRENT``
******************

The phase current has exceeded the overcurrent threshold. The overcurrent threshold is 1,5 times the user-defined current limit, and in any case no more than 50A.

5: ``PWM_LIMIT_EXCEEDED``
*************************

This is a legacy error code that is not in use.

6: ``PHASE_RESISTANCE_OUT_OF_RANGE``
************************************

The phase resistance measured during calibration is out of range. The defined range is 5mΩ to 1Ω.

7: ``PHASE_INDUCTANCE_OUT_OF_RANGE``
************************************

The phase inductance measured during calibration is out of range. The defined range is 2μH to 5mH.

8: ``INVALID_POLE_PAIRS``
*************************

The pole pair detection algorithm did not converge near an integer number during calibration.

9: ``ENCODER_READING_UNSTABLE``
*******************************

Encoder reading variation is over maximum allowed threshold. This is usually the casse if the magnet is misaligned, too far away from the encoder IC, or missing.
