
.. _api-reference:

API REFERENCE
=============

Spec version 1.0.1


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


calibrated
-------------------------------------------------------------------

ID: 4
Type: bool


Whether the system has been calibrated.


errors
-------------------------------------------------------------------

ID: 5
Type: uint8


Any system errors, as a bitmask


save_config() -> void
-------------------------------------------------------------------

ID: 6
Return Type: void


Save configuration to non-volatile memory.


erase_config() -> void
-------------------------------------------------------------------

ID: 7
Return Type: void


Erase the config stored in non-volatile memory and reset the device.


reset() -> void
-------------------------------------------------------------------

ID: 8
Return Type: void


Reset the device.


scheduler.total
-------------------------------------------------------------------

ID: 9
Type: uint32


Total processor cycles in a single PWM cycle.


scheduler.busy
-------------------------------------------------------------------

ID: 10
Type: uint32


Busy processor cycles in a single PWM cycle.


scheduler.errors
-------------------------------------------------------------------

ID: 11
Type: uint8


Any scheduler errors, as a bitmask


controller.state
-------------------------------------------------------------------

ID: 12
Type: uint8


The state of the controller.


controller.mode
-------------------------------------------------------------------

ID: 13
Type: uint8


The control mode of the controller.


controller.warnings
-------------------------------------------------------------------

ID: 14
Type: uint8


Any controller warnings, as a bitmask


controller.errors
-------------------------------------------------------------------

ID: 15
Type: uint8


Any controller errors, as a bitmask


controller.position.setpoint
-------------------------------------------------------------------

ID: 16
Type: float
Units: tick

The position setpoint.


controller.position.p_gain
-------------------------------------------------------------------

ID: 17
Type: float


The proportional gain of the position controller.


controller.velocity.setpoint
-------------------------------------------------------------------

ID: 18
Type: float
Units: tick / second

The velocity setpoint.


controller.velocity.limit
-------------------------------------------------------------------

ID: 19
Type: float
Units: tick / second

The velocity limit.


controller.velocity.p_gain
-------------------------------------------------------------------

ID: 20
Type: float


The proportional gain of the velocity controller.


controller.velocity.i_gain
-------------------------------------------------------------------

ID: 21
Type: float


The integral gain of the velocity controller.


.. _integrator-deadband:

controller.velocity.deadband
-------------------------------------------------------------------

ID: 22
Type: float
Units: tick

The deadband of the velocity integrator. A region around the position setpoint where the velocity integrator is not updated.


controller.velocity.increment
-------------------------------------------------------------------

ID: 23
Type: float


Max velocity setpoint increment (ramping) rate. Set to 0 to disable.


controller.current.Iq_setpoint
-------------------------------------------------------------------

ID: 24
Type: float
Units: ampere

The Iq setpoint.


controller.current.Iq_limit
-------------------------------------------------------------------

ID: 25
Type: float
Units: ampere

The Iq limit.


controller.current.Iq_estimate
-------------------------------------------------------------------

ID: 26
Type: float
Units: ampere

The Iq estimate.


controller.current.bandwidth
-------------------------------------------------------------------

ID: 27
Type: float


The current controller bandwidth.


controller.current.Iq_p_gain
-------------------------------------------------------------------

ID: 28
Type: float


The current controller proportional gain.


controller.current.max_Iq_feedback
-------------------------------------------------------------------

ID: 29
Type: float
Units: ampere

The max current allowed to be fed back to the power source before flux braking activates.


controller.current.max_Id_dump
-------------------------------------------------------------------

ID: 30
Type: float
Units: ampere

The max current allowed to be dumped to the motor windings during flux braking. Set to zero to deactivate flux braking.


controller.voltage.Vq_setpoint
-------------------------------------------------------------------

ID: 31
Type: float
Units: volt

The Vq setpoint.


calibrate() -> void
-------------------------------------------------------------------

ID: 32
Return Type: void


Calibrate the device.


idle() -> void
-------------------------------------------------------------------

ID: 33
Return Type: void


Set idle mode, disabling the driver.


position_mode() -> void
-------------------------------------------------------------------

ID: 34
Return Type: void


Set position control mode.


velocity_mode() -> void
-------------------------------------------------------------------

ID: 35
Return Type: void


Set velocity control mode.


current_mode() -> void
-------------------------------------------------------------------

ID: 36
Return Type: void


Set current control mode.


set_pos_vel_setpoints(pos_setpoint, vel_setpoint) -> float
-------------------------------------------------------------------

ID: 37
Return Type: float


Set the position and velocity setpoints in one go, and retrieve the position estimate


.. _api-can-rate:

comms.can.rate
-------------------------------------------------------------------

ID: 38
Type: uint32


The baud rate of the CAN interface.


comms.can.id
-------------------------------------------------------------------

ID: 39
Type: uint32


The ID of the CAN interface.


motor.R
-------------------------------------------------------------------

ID: 40
Type: float
Units: ohm

The motor Resistance value.


motor.L
-------------------------------------------------------------------

ID: 41
Type: float
Units: henry

The motor Inductance value.


motor.pole_pairs
-------------------------------------------------------------------

ID: 42
Type: uint8


The motor pole pair count.


motor.type
-------------------------------------------------------------------

ID: 43
Type: uint8


The type of the motor. Either high current or gimbal.


motor.offset
-------------------------------------------------------------------

ID: 44
Type: float


User-defined offset of the motor.


motor.direction
-------------------------------------------------------------------

ID: 45
Type: int8


User-defined direction of the motor.


motor.calibrated
-------------------------------------------------------------------

ID: 46
Type: bool


Whether the motor has been calibrated.


motor.I_cal
-------------------------------------------------------------------

ID: 47
Type: float
Units: ampere

The calibration current.


motor.errors
-------------------------------------------------------------------

ID: 48
Type: uint8


Any motor/calibration errors, as a bitmask


encoder.position_estimate
-------------------------------------------------------------------

ID: 49
Type: float
Units: tick

The filtered encoder position estimate.


encoder.velocity_estimate
-------------------------------------------------------------------

ID: 50
Type: float
Units: tick / second

The filtered encoder velocity estimate.


encoder.type
-------------------------------------------------------------------

ID: 51
Type: uint8


The encoder type. Either INTERNAL or HALL.


encoder.bandwidth
-------------------------------------------------------------------

ID: 52
Type: float
Units: radian / second

The encoder observer bandwidth.


encoder.calibrated
-------------------------------------------------------------------

ID: 53
Type: bool


Whether the encoder has been calibrated.


encoder.errors
-------------------------------------------------------------------

ID: 54
Type: uint8


Any encoder errors, as a bitmask


traj_planner.max_accel
-------------------------------------------------------------------

ID: 55
Type: float
Units: tick / second

The trajectory planner max acceleration.


traj_planner.max_decel
-------------------------------------------------------------------

ID: 56
Type: float
Units: tick / second ** 2

The trajectory planner max deceleration.


traj_planner.max_vel
-------------------------------------------------------------------

ID: 57
Type: float
Units: tick / second

The trajectory planner max cruise velocity.


move_to(pos_setpoint) -> void
-------------------------------------------------------------------

ID: 58
Return Type: void


Move to target position respecting velocity and acceleration limits.


move_to_tlimit(pos_setpoint) -> void
-------------------------------------------------------------------

ID: 59
Return Type: void


Move to target position respecting time limits for each sector.


traj_planner.errors
-------------------------------------------------------------------

ID: 60
Type: uint8


Any errors in the trajectory planner, as a bitmask


watchdog.enabled
-------------------------------------------------------------------

ID: 61
Type: bool


Whether the watchdog is enabled or not.


watchdog.triggered
-------------------------------------------------------------------

ID: 62
Type: bool


Whether the watchdog has been triggered or not.


watchdog.timeout
-------------------------------------------------------------------

ID: 63
Type: float
Units: second

The watchdog timeout period.


