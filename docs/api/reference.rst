
API REFERENCE
=============

Spec version 0.9.0


uid
-------------------------------------------------------------------

ID: 0
Type: uint32
Units: 

The unique device ID, unique to each PAC55xx chip produced.


Vbus
-------------------------------------------------------------------

ID: 1
Type: float


The bus voltage.


temp
-------------------------------------------------------------------

ID: 2
Type: float


The internal temperature of the PAC55xx MCU.


reset() -> void
-------------------------------------------------------------------

ID: 3
Return Type: void


Reset the device.


total
-------------------------------------------------------------------

ID: 4
Type: uint32
Units: 

Total processor cycles in a single PWM cycle.


busy
-------------------------------------------------------------------

ID: 5
Type: uint32
Units: 

Busy processor cycles in a single PWM cycle.


state
-------------------------------------------------------------------

ID: 6
Type: uint8
Units: 

The state of the controller.


mode
-------------------------------------------------------------------

ID: 7
Type: uint8
Units: 

The control mode of the controller.


pos_setpoint
-------------------------------------------------------------------

ID: 8
Type: float
Units: 

The position setpoint.


vel_setpoint
-------------------------------------------------------------------

ID: 9
Type: float
Units: 

The velocity setpoint.


Iq_setpoint
-------------------------------------------------------------------

ID: 10
Type: float
Units: 

The Iq setpoint.


Iq_estimate
-------------------------------------------------------------------

ID: 11
Type: float
Units: 

The Iq estimate.


pos_p_gain
-------------------------------------------------------------------

ID: 12
Type: float
Units: 

The proportional gain of the position controller.


vel_p_gain
-------------------------------------------------------------------

ID: 13
Type: float
Units: 

The proportional gain of the velocity controller.


vel_i_gain
-------------------------------------------------------------------

ID: 14
Type: float
Units: 

The integral gain of the velocity controller.


Iq_p_gain
-------------------------------------------------------------------

ID: 15
Type: float
Units: 

The current controller proportional gain.


gain
-------------------------------------------------------------------

ID: 16
Type: float
Units: 

The gain of the velocity integrator.


deadband
-------------------------------------------------------------------

ID: 17
Type: float
Units: 

The deadband of the velocity integrator. A region around the position setpoint where the velocity integrator is not updated.


rate
-------------------------------------------------------------------

ID: 18
Type: uint32
Units: 

The baud rate of the CAN interface.


id
-------------------------------------------------------------------

ID: 19
Type: uint32
Units: 

The ID of the CAN interface.


R
-------------------------------------------------------------------

ID: 20
Type: float


The motor Resistance value.


L
-------------------------------------------------------------------

ID: 21
Type: float


The motor Inductance value.


pole_pairs
-------------------------------------------------------------------

ID: 22
Type: uint8
Units: 

The motor pole pair count.


type
-------------------------------------------------------------------

ID: 23
Type: uint8
Units: 

The type of the motor. Either high current or gimbal.


offset
-------------------------------------------------------------------

ID: 24
Type: float
Units: 

User-defined offset of the motor.


direction
-------------------------------------------------------------------

ID: 25
Type: uint8
Units: 

User-defined direction of the motor.


position_estimate
-------------------------------------------------------------------

ID: 26
Type: float


The filtered encoder position estimate.


type
-------------------------------------------------------------------

ID: 27
Type: uint8
Units: 

The encoder type. Either INTERNAL or HALL.


bandwidth
-------------------------------------------------------------------

ID: 28
Type: float


The encoder observer bandwidth.


max_accel
-------------------------------------------------------------------

ID: 29
Type: float


The trajectory planner max acceleration.


max_decel
-------------------------------------------------------------------

ID: 30
Type: float


The trajectory planner max deceleration.


max_vel
-------------------------------------------------------------------

ID: 31
Type: float


The trajectory planner max cruise velocity.


pos_setpoint
-------------------------------------------------------------------

ID: 32
Type: float


The position setpoint of the trajectory planner.


