.. _api-reference:

API Reference
-------------

tm.uid
------

- Endpoint ID: 1
- Data Type: uint32
- Unit: Not defined

Retrieves the unique device ID.


tm.Vbus
-------

- Endpoint ID: 2
- Data Type: float
- Unit: volt

Retrieves the bus voltage.


tm.motor.R
----------

- Endpoint ID: 3
- Data Type: float
- Unit: ohm

Accesses the motor Resistance value.


tm.motor.L
----------

- Endpoint ID: 4
- Data Type: float
- Unit: henry

Accesses the motor Inductance value.


tm.motor.pole_pairs
-------------------

- Endpoint ID: 5
- Data Type: uint8
- Unit: Not defined

Accesses the motor pole pair count.


tm.motor.type
-------------

- Endpoint ID: 6
- Data Type: uint8
- Unit: Not defined

Accesses the motor type.


tm.encoder.position_estimate
----------------------------

- Endpoint ID: 7
- Data Type: float
- Unit: tick

Retrieves the encoder position estimate.


tm.encoder.bandwidth
--------------------

- Endpoint ID: 8
- Data Type: float
- Unit: radian / second

Accesses the encoder observer bandwidth.

