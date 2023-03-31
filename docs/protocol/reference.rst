
.. _api-reference:

API REFERENCE
=============

Spec version 1.0.1.b1


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

The measured bus voltage.




Ibus
-------------------------------------------------------------------

ID: 3
Type: float
Units: ampere

The estimated bus current. Only estimates current drawn by motor.




power
-------------------------------------------------------------------

ID: 4
Type: float
Units: watt

The estimated power. Only estimates power drawn by motor.




temp
-------------------------------------------------------------------

ID: 5
Type: float
Units: degree_Celsius

The internal temperature of the PAC55xx MCU.




calibrated
-------------------------------------------------------------------

ID: 6
Type: bool


Whether the system has been calibrated.




errors
-------------------------------------------------------------------

ID: 7
Type: uint8


Any system errors, as a bitmask

Flags: 
- UNDERVOLTAGE
- DRIVER_FAULT


save_config() -> void
-------------------------------------------------------------------

ID: 8
Return Type: void


Save configuration to non-volatile memory.


erase_config() -> void
-------------------------------------------------------------------

ID: 9
Return Type: void


Erase the config stored in non-volatile memory and reset the device.


reset() -> void
-------------------------------------------------------------------

ID: 10
Return Type: void


Reset the device.


scheduler.total
-------------------------------------------------------------------

ID: 11
Type: uint32


Total processor cycles in a single PWM cycle.




scheduler.busy
-------------------------------------------------------------------

ID: 12
Type: uint32


Busy processor cycles in a single PWM cycle.




scheduler.errors
-------------------------------------------------------------------

ID: 13
Type: uint8


Any scheduler errors, as a bitmask

Flags: 
- CONTROL_BLOCK_REENTERED


controller.state
-------------------------------------------------------------------

ID: 14
Type: uint8


The state of the controller.




controller.mode
-------------------------------------------------------------------

ID: 15
Type: uint8


The control mode of the controller.




controller.warnings
-------------------------------------------------------------------

ID: 16
Type: uint8


Any controller warnings, as a bitmask

Flags: 
- VELOCITY_LIMITED
- CURRENT_LIMITED
- MODULATION_LIMITED


controller.errors
-------------------------------------------------------------------

ID: 17
Type: uint8


Any controller errors, as a bitmask

Flags: 
- CURRENT_LIMIT_EXCEEDED


controller.position.setpoint
-------------------------------------------------------------------

ID: 18
Type: float
Units: tick

The position setpoint.




controller.position.p_gain
-------------------------------------------------------------------

ID: 19
Type: float


The proportional gain of the position controller.




controller.velocity.setpoint
-------------------------------------------------------------------

ID: 20
Type: float
Units: tick / second

The velocity setpoint.




controller.velocity.limit
-------------------------------------------------------------------

ID: 21
Type: float
Units: tick / second

The velocity limit.




controller.velocity.p_gain
-------------------------------------------------------------------

ID: 22
Type: float


The proportional gain of the velocity controller.




controller.velocity.i_gain
-------------------------------------------------------------------

ID: 23
Type: float


The integral gain of the velocity controller.




.. _integrator-deadband:

controller.velocity.deadband
-------------------------------------------------------------------

ID: 24
Type: float
Units: tick

The deadband of the velocity integrator. A region around the position setpoint where the velocity integrator is not updated.




controller.velocity.increment
-------------------------------------------------------------------

ID: 25
Type: float


Max velocity setpoint increment (ramping) rate. Set to 0 to disable.




controller.current.Iq_setpoint
-------------------------------------------------------------------

ID: 26
Type: float
Units: ampere

The Iq setpoint.




controller.current.Id_setpoint
-------------------------------------------------------------------

ID: 27
Type: float
Units: ampere

The Id setpoint.




controller.current.Iq_limit
-------------------------------------------------------------------

ID: 28
Type: float
Units: ampere

The Iq limit.




controller.current.Iq_estimate
-------------------------------------------------------------------

ID: 29
Type: float
Units: ampere

The Iq estimate.




controller.current.bandwidth
-------------------------------------------------------------------

ID: 30
Type: float
Units: radian / second

The current controller bandwidth.




controller.current.Iq_p_gain
-------------------------------------------------------------------

ID: 31
Type: float


The current controller proportional gain.




controller.current.max_Ibus_regen
-------------------------------------------------------------------

ID: 32
Type: float
Units: ampere

The max current allowed to be fed back to the power source before flux braking activates.




controller.current.max_Ibrake
-------------------------------------------------------------------

ID: 33
Type: float
Units: ampere

The max current allowed to be dumped to the motor windings during flux braking. Set to zero to deactivate flux braking.




controller.voltage.Vq_setpoint
-------------------------------------------------------------------

ID: 34
Type: float
Units: volt

The Vq setpoint.




calibrate() -> void
-------------------------------------------------------------------

ID: 35
Return Type: void


Calibrate the device.


idle() -> void
-------------------------------------------------------------------

ID: 36
Return Type: void


Set idle mode, disabling the driver.


position_mode() -> void
-------------------------------------------------------------------

ID: 37
Return Type: void


Set position control mode.


velocity_mode() -> void
-------------------------------------------------------------------

ID: 38
Return Type: void


Set velocity control mode.


current_mode() -> void
-------------------------------------------------------------------

ID: 39
Return Type: void


Set current control mode.


set_pos_vel_setpoints(pos_setpoint, vel_setpoint) -> float
-------------------------------------------------------------------

ID: 40
Return Type: float


Set the position and velocity setpoints in one go, and retrieve the position estimate


.. _api-can-rate:

comms.can.rate
-------------------------------------------------------------------

ID: 41
Type: uint32


The baud rate of the CAN interface.




comms.can.id
-------------------------------------------------------------------

ID: 42
Type: uint32


The ID of the CAN interface.




motor.R
-------------------------------------------------------------------

ID: 43
Type: float
Units: ohm

The motor Resistance value.




motor.L
-------------------------------------------------------------------

ID: 44
Type: float
Units: henry

The motor Inductance value.




motor.pole_pairs
-------------------------------------------------------------------

ID: 45
Type: uint8


The motor pole pair count.




motor.type
-------------------------------------------------------------------

ID: 46
Type: uint8


The type of the motor. Either high current or gimbal.




motor.offset
-------------------------------------------------------------------

ID: 47
Type: float


User-defined offset of the motor.




motor.direction
-------------------------------------------------------------------

ID: 48
Type: int8


User-defined direction of the motor.




motor.calibrated
-------------------------------------------------------------------

ID: 49
Type: bool


Whether the motor has been calibrated.




motor.I_cal
-------------------------------------------------------------------

ID: 50
Type: float
Units: ampere

The calibration current.




motor.errors
-------------------------------------------------------------------

ID: 51
Type: uint8


Any motor/calibration errors, as a bitmask

Flags: 
- PHASE_RESISTANCE_OUT_OF_RANGE
- PHASE_INDUCTANCE_OUT_OF_RANGE
- INVALID_POLE_PAIRS


encoder.position_estimate
-------------------------------------------------------------------

ID: 52
Type: float
Units: tick

The filtered encoder position estimate.




encoder.velocity_estimate
-------------------------------------------------------------------

ID: 53
Type: float
Units: tick / second

The filtered encoder velocity estimate.




encoder.type
-------------------------------------------------------------------

ID: 54
Type: uint8


The encoder type. Either INTERNAL or HALL.




encoder.bandwidth
-------------------------------------------------------------------

ID: 55
Type: float
Units: radian / second

The encoder observer bandwidth.




encoder.calibrated
-------------------------------------------------------------------

ID: 56
Type: bool


Whether the encoder has been calibrated.




encoder.errors
-------------------------------------------------------------------

ID: 57
Type: uint8


Any encoder errors, as a bitmask

Flags: 
- CALIBRATION_FAILED
- READING_UNSTABLE


traj_planner.max_accel
-------------------------------------------------------------------

ID: 58
Type: float
Units: tick / second

The trajectory planner max acceleration.




traj_planner.max_decel
-------------------------------------------------------------------

ID: 59
Type: float
Units: tick / second ** 2

The trajectory planner max deceleration.




traj_planner.max_vel
-------------------------------------------------------------------

ID: 60
Type: float
Units: tick / second

The trajectory planner max cruise velocity.




move_to(pos_setpoint) -> void
-------------------------------------------------------------------

ID: 61
Return Type: void


Move to target position respecting velocity and acceleration limits.


move_to_tlimit(pos_setpoint) -> void
-------------------------------------------------------------------

ID: 62
Return Type: void


Move to target position respecting time limits for each sector.


traj_planner.errors
-------------------------------------------------------------------

ID: 63
Type: uint8


Any errors in the trajectory planner, as a bitmask

Flags: 
- INVALID_INPUT
- VCRUISE_OVER_LIMIT


watchdog.enabled
-------------------------------------------------------------------

ID: 64
Type: bool


Whether the watchdog is enabled or not.




watchdog.triggered
-------------------------------------------------------------------

ID: 65
Type: bool


Whether the watchdog has been triggered or not.




watchdog.timeout
-------------------------------------------------------------------

ID: 66
Type: float
Units: second

The watchdog timeout period.




