.. _api-reference:

API Reference
-------------

tm.uid
------

- Endpoint ID: 1
- Data Type: uint32
- Unit: Not defined

The unique device ID, unique to each PAC55xx chip produced.


tm.Vbus
-------

- Endpoint ID: 2
- Data Type: float
- Unit: volt

The bus voltage.


tm.temp
-------

- Endpoint ID: 3
- Data Type: float
- Unit: degree_Celsius

The internal temperature of the PAC55xx MCU.


tm.controller.state
-------------------

- Endpoint ID: 4
- Data Type: uint8
- Unit: Not defined

The state of the controller.


tm.controller.mode
------------------

- Endpoint ID: 5
- Data Type: uint8
- Unit: Not defined

The control mode of the controller.


tm.controller.pos_setpoint
--------------------------

- Endpoint ID: 6
- Data Type: float
- Unit: Not defined

The position setpoint.


tm.controller.vel_setpoint
--------------------------

- Endpoint ID: 7
- Data Type: float
- Unit: Not defined

The velocity setpoint.


tm.controller.Iq_setpoint
-------------------------

- Endpoint ID: 8
- Data Type: float
- Unit: Not defined

The Iq setpoint.


tm.controller.Iq_estimate
-------------------------

- Endpoint ID: 9
- Data Type: float
- Unit: Not defined

The Iq estimate.


tm.controller.pos_p_gain
------------------------

- Endpoint ID: 10
- Data Type: float
- Unit: Not defined

The proportional gain of the position controller.


tm.controller.vel_p_gain
------------------------

- Endpoint ID: 11
- Data Type: float
- Unit: Not defined

The proportional gain of the velocity controller.


tm.controller.vel_i_gain
------------------------

- Endpoint ID: 12
- Data Type: float
- Unit: Not defined

The integral gain of the velocity controller.


tm.controller.Iq_p_gain
-----------------------

- Endpoint ID: 13
- Data Type: float
- Unit: Not defined

The current controller proportional gain.


tm.controller.vel_integrator.gain
---------------------------------

- Endpoint ID: 14
- Data Type: float
- Unit: Not defined

The gain of the velocity integrator.


.. _integrator-deadband:

tm.controller.vel_integrator.deadband
-------------------------------------

- Endpoint ID: 15
- Data Type: float
- Unit: Not defined

The deadband of the velocity integrator. A region around the position
setpoint where the velocity integrator is not updated.


.. _api-can-rate:

tm.comms.can.rate
-----------------

- Endpoint ID: 16
- Data Type: uint32
- Unit: Not defined

The baud rate of the CAN interface.


tm.comms.can.id
---------------

- Endpoint ID: 17
- Data Type: uint32
- Unit: Not defined

The ID of the CAN interface.


tm.motor.R
----------

- Endpoint ID: 18
- Data Type: float
- Unit: ohm

The motor Resistance value.


tm.motor.L
----------

- Endpoint ID: 19
- Data Type: float
- Unit: henry

The motor Inductance value.


tm.motor.pole_pairs
-------------------

- Endpoint ID: 20
- Data Type: uint8
- Unit: Not defined

The motor pole pair count.


tm.motor.type
-------------

- Endpoint ID: 21
- Data Type: uint8
- Unit: Not defined

The type of the motor. Either high current or gimbal.


tm.encoder.position_estimate
----------------------------

- Endpoint ID: 22
- Data Type: float
- Unit: tick

The filtered encoder position estimate.


tm.encoder.bandwidth
--------------------

- Endpoint ID: 23
- Data Type: float
- Unit: radian / second

The encoder observer bandwidth.

