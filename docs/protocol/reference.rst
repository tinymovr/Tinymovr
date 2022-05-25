
.. _api-reference:

API REFERENCE
=============

Spec version 0.9.0


protocol_hash
-------------------------------------------------------------------

ID: 0
Type: uint32


The Avlos protocol hash.


uid
-------------------------------------------------------------------

ID: 1
Type: uint32


The unique device ID, unique to each PAC55xx chip produced.


Vbus
-------------------------------------------------------------------

ID: 2
Type: float
Units: volt

The bus voltage.


temp
-------------------------------------------------------------------

ID: 3
Type: float
Units: degree_Celsius

The internal temperature of the PAC55xx MCU.


save_config() -> void
-------------------------------------------------------------------

ID: 4
Return Type: void


Save configuration to non-volatile memory.


erase_config() -> void
-------------------------------------------------------------------

ID: 5
Return Type: void


Erase the config stored in non-volatile memory and reset the device.


reset() -> void
-------------------------------------------------------------------

ID: 6
Return Type: void


Reset the device.


cycles.total
-------------------------------------------------------------------

ID: 7
Type: uint32


Total processor cycles in a single PWM cycle.


cycles.busy
-------------------------------------------------------------------

ID: 8
Type: uint32


Busy processor cycles in a single PWM cycle.


controller.state
-------------------------------------------------------------------

ID: 9
Type: uint8


The state of the controller.


controller.mode
-------------------------------------------------------------------

ID: 10
Type: uint8


The control mode of the controller.


controller.pos_setpoint
-------------------------------------------------------------------

ID: 11
Type: float
Units: tick

The position setpoint.


controller.vel_setpoint
-------------------------------------------------------------------

ID: 12
Type: float
Units: tick / second

The velocity setpoint.


controller.Iq_setpoint
-------------------------------------------------------------------

ID: 13
Type: float
Units: ampere

The Iq setpoint.


controller.vel_limit
-------------------------------------------------------------------

ID: 14
Type: float
Units: tick / second

The velocity limit.


controller.Iq_limit
-------------------------------------------------------------------

ID: 15
Type: float
Units: ampere

The Iq limit.


controller.Iq_estimate
-------------------------------------------------------------------

ID: 16
Type: float
Units: ampere

The Iq estimate.


controller.pos_p_gain
-------------------------------------------------------------------

ID: 17
Type: float


The proportional gain of the position controller.


controller.vel_p_gain
-------------------------------------------------------------------

ID: 18
Type: float


The proportional gain of the velocity controller.


controller.vel_i_gain
-------------------------------------------------------------------

ID: 19
Type: float


The integral gain of the velocity controller.


controller.I_bw
-------------------------------------------------------------------

ID: 20
Type: float


The current controller bandwidth.


controller.Iq_p_gain
-------------------------------------------------------------------

ID: 21
Type: float


The current controller proportional gain.


controller.vel_integrator.gain
-------------------------------------------------------------------

ID: 22
Type: float


The gain of the velocity integrator.


controller.vel_integrator.deadband
-------------------------------------------------------------------

ID: 23
Type: float


The deadband of the velocity integrator. A region around the position setpoint where the velocity integrator is not updated.


calibrate() -> void
-------------------------------------------------------------------

ID: 24
Return Type: void


Calibrate the device.


idle() -> void
-------------------------------------------------------------------

ID: 25
Return Type: void


Set idle mode, disabling the driver.


position_mode() -> void
-------------------------------------------------------------------

ID: 26
Return Type: void


Set position control mode.


velocity_mode() -> void
-------------------------------------------------------------------

ID: 27
Return Type: void


Set velocity control mode.


current_mode() -> void
-------------------------------------------------------------------

ID: 28
Return Type: void


Set current control mode.


comms.can.rate
-------------------------------------------------------------------

ID: 29
Type: uint32


The baud rate of the CAN interface.


comms.can.id
-------------------------------------------------------------------

ID: 30
Type: uint32


The ID of the CAN interface.


motor.R
-------------------------------------------------------------------

ID: 31
Type: float
Units: ohm

The motor Resistance value.


motor.L
-------------------------------------------------------------------

ID: 32
Type: float
Units: henry

The motor Inductance value.


motor.pole_pairs
-------------------------------------------------------------------

ID: 33
Type: uint8


The motor pole pair count.


motor.type
-------------------------------------------------------------------

ID: 34
Type: uint8


The type of the motor. Either high current or gimbal.


motor.offset
-------------------------------------------------------------------

ID: 35
Type: float


User-defined offset of the motor.


motor.direction
-------------------------------------------------------------------

ID: 36
Type: uint8


User-defined direction of the motor.


encoder.position_estimate
-------------------------------------------------------------------

ID: 37
Type: float
Units: tick

The filtered encoder position estimate.


encoder.type
-------------------------------------------------------------------

ID: 38
Type: uint8


The encoder type. Either INTERNAL or HALL.


encoder.bandwidth
-------------------------------------------------------------------

ID: 39
Type: float
Units: radian / second

The encoder observer bandwidth.


traj_planner.max_accel
-------------------------------------------------------------------

ID: 40
Type: float
Units: tick / second

The trajectory planner max acceleration.


traj_planner.max_decel
-------------------------------------------------------------------

ID: 41
Type: float
Units: tick / second ** 2

The trajectory planner max deceleration.


traj_planner.max_vel
-------------------------------------------------------------------

ID: 42
Type: float
Units: tick / second

The trajectory planner max cruise velocity.


move_to(pos_setpoint) -> void
-------------------------------------------------------------------

ID: 43
Return Type: void


Move to target position respecting velocity and acceleration limits.


move_to_tlimit(pos_setpoint) -> void
-------------------------------------------------------------------

ID: 44
Return Type: void


Move to target position respecting time limits for each sector.


