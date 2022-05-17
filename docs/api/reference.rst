
API REFERENCE
=============

Spec version 0.9.0


uid
-------------------------------------------------------------------

ID: 0
Type: uint32


The unique device ID, unique to each PAC55xx chip produced.


Vbus
-------------------------------------------------------------------

ID: 1
Type: float
Units: volt

The bus voltage.


temp
-------------------------------------------------------------------

ID: 2
Type: float
Units: degree_Celsius

The internal temperature of the PAC55xx MCU.


save_config() -> void
-------------------------------------------------------------------

ID: 3
Return Type: void


Save configuration to non-volatile memory.


erase_config() -> void
-------------------------------------------------------------------

ID: 4
Return Type: void


Erase the config stored in non-volatile memory and reset the device.


reset() -> void
-------------------------------------------------------------------

ID: 5
Return Type: void


Reset the device.


total
-------------------------------------------------------------------

ID: 6
Type: uint32


Total processor cycles in a single PWM cycle.


busy
-------------------------------------------------------------------

ID: 7
Type: uint32


Busy processor cycles in a single PWM cycle.


state
-------------------------------------------------------------------

ID: 8
Type: uint8


The state of the controller.


mode
-------------------------------------------------------------------

ID: 9
Type: uint8


The control mode of the controller.


pos_setpoint
-------------------------------------------------------------------

ID: 10
Type: float
Units: tick

The position setpoint.


vel_setpoint
-------------------------------------------------------------------

ID: 11
Type: float
Units: tick / second

The velocity setpoint.


Iq_setpoint
-------------------------------------------------------------------

ID: 12
Type: float
Units: ampere

The Iq setpoint.


Iq_estimate
-------------------------------------------------------------------

ID: 13
Type: float
Units: ampere

The Iq estimate.


pos_p_gain
-------------------------------------------------------------------

ID: 14
Type: float


The proportional gain of the position controller.


vel_p_gain
-------------------------------------------------------------------

ID: 15
Type: float


The proportional gain of the velocity controller.


vel_i_gain
-------------------------------------------------------------------

ID: 16
Type: float


The integral gain of the velocity controller.


Iq_p_gain
-------------------------------------------------------------------

ID: 17
Type: float


The current controller proportional gain.


gain
-------------------------------------------------------------------

ID: 18
Type: float


The gain of the velocity integrator.


deadband
-------------------------------------------------------------------

ID: 19
Type: float


The deadband of the velocity integrator. A region around the position setpoint where the velocity integrator is not updated.


rate
-------------------------------------------------------------------

ID: 20
Type: uint32


The baud rate of the CAN interface.


id
-------------------------------------------------------------------

ID: 21
Type: uint32


The ID of the CAN interface.


R
-------------------------------------------------------------------

ID: 22
Type: float
Units: ohm

The motor Resistance value.


L
-------------------------------------------------------------------

ID: 23
Type: float
Units: henry

The motor Inductance value.


pole_pairs
-------------------------------------------------------------------

ID: 24
Type: uint8


The motor pole pair count.


type
-------------------------------------------------------------------

ID: 25
Type: uint8


The type of the motor. Either high current or gimbal.


offset
-------------------------------------------------------------------

ID: 26
Type: float


User-defined offset of the motor.


direction
-------------------------------------------------------------------

ID: 27
Type: uint8


User-defined direction of the motor.


position_estimate
-------------------------------------------------------------------

ID: 28
Type: float
Units: tick

The filtered encoder position estimate.


type
-------------------------------------------------------------------

ID: 29
Type: uint8


The encoder type. Either INTERNAL or HALL.


bandwidth
-------------------------------------------------------------------

ID: 30
Type: float
Units: radian / second

The encoder observer bandwidth.


max_accel
-------------------------------------------------------------------

ID: 31
Type: float
Units: tick / second

The trajectory planner max acceleration.


max_decel
-------------------------------------------------------------------

ID: 32
Type: float
Units: tick / second ** 2

The trajectory planner max deceleration.


max_vel
-------------------------------------------------------------------

ID: 33
Type: float
Units: tick / second

The trajectory planner max cruise velocity.


pos_setpoint
-------------------------------------------------------------------

ID: 34
Type: float
Units: tick

The position setpoint of the trajectory planner.


