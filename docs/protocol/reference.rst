
.. _api-reference:

API REFERENCE
=============



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



fw_version
-------------------------------------------------------------------

ID: 2

Type: str



The firmware version.



hw_revision
-------------------------------------------------------------------

ID: 3

Type: uint32



The hardware revision.



Vbus
-------------------------------------------------------------------

ID: 4

Type: float

Units: volt

The measured bus voltage.



Ibus
-------------------------------------------------------------------

ID: 5

Type: float

Units: ampere

The estimated bus current. Only estimates current drawn by motor.



power
-------------------------------------------------------------------

ID: 6

Type: float

Units: watt

The estimated power. Only estimates power drawn by motor.



temp
-------------------------------------------------------------------

ID: 7

Type: float

Units: degree_Celsius

The internal temperature of the PAC55xx MCU.



calibrated
-------------------------------------------------------------------

ID: 8

Type: bool



Whether the system has been calibrated.



errors
-------------------------------------------------------------------

ID: 9

Type: uint8



Any system errors, as a bitmask

Flags: 

- UNDERVOLTAGE

warnings
-------------------------------------------------------------------

ID: 10

Type: uint8



Any system warnings, as a bitmask

Flags: 

- DRIVER_FAULT

- CHARGE_PUMP_FAULT_STAT

- CHARGE_PUMP_FAULT

- DRV10_DISABLE

- DRV32_DISABLE

- DRV54_DISABLE

save_config() -> void
--------------------------------------------------------------------------------------------

ID: 11

Return Type: void



Save configuration to non-volatile memory.

erase_config() -> void
--------------------------------------------------------------------------------------------

ID: 12

Return Type: void



Erase the config stored in non-volatile memory and reset the device.

reset() -> void
--------------------------------------------------------------------------------------------

ID: 13

Return Type: void



Reset the device.

enter_dfu() -> void
--------------------------------------------------------------------------------------------

ID: 14

Return Type: void



Enter DFU mode.

config_size
-------------------------------------------------------------------

ID: 15

Type: uint32



Size (in bytes) of the configuration object.



scheduler.load
-------------------------------------------------------------------

ID: 16

Type: uint32



Processor load in ticks per PWM cycle.



scheduler.warnings
-------------------------------------------------------------------

ID: 17

Type: uint8



Any scheduler warnings, as a bitmask

Flags: 

- CONTROL_BLOCK_REENTERED

controller.state
-------------------------------------------------------------------

ID: 18

Type: uint8



The state of the controller.

Options: 

- IDLE

- CALIBRATE

- CL_CONTROL

controller.mode
-------------------------------------------------------------------

ID: 19

Type: uint8



The control mode of the controller.

Options: 

- CURRENT

- VELOCITY

- POSITION

- TRAJECTORY

- HOMING

controller.warnings
-------------------------------------------------------------------

ID: 20

Type: uint8



Any controller warnings, as a bitmask

Flags: 

- VELOCITY_LIMITED

- CURRENT_LIMITED

- MODULATION_LIMITED

controller.errors
-------------------------------------------------------------------

ID: 21

Type: uint8



Any controller errors, as a bitmask

Flags: 

- CURRENT_LIMIT_EXCEEDED

controller.position.setpoint
-------------------------------------------------------------------

ID: 22

Type: float

Units: tick

The position setpoint in the user reference frame.



controller.position.p_gain
-------------------------------------------------------------------

ID: 23

Type: float



The proportional gain of the position controller.



controller.velocity.setpoint
-------------------------------------------------------------------

ID: 24

Type: float

Units: tick / second

The velocity setpoint in the user reference frame.



controller.velocity.limit
-------------------------------------------------------------------

ID: 25

Type: float

Units: tick / second

The velocity limit.



controller.velocity.p_gain
-------------------------------------------------------------------

ID: 26

Type: float



The proportional gain of the velocity controller.



controller.velocity.i_gain
-------------------------------------------------------------------

ID: 27

Type: float



The integral gain of the velocity controller.



.. _integrator-deadband:

controller.velocity.deadband
-------------------------------------------------------------------

ID: 28

Type: float

Units: tick

The deadband of the velocity integrator. A region around the position setpoint where the velocity integrator is not updated.



controller.velocity.increment
-------------------------------------------------------------------

ID: 29

Type: float



Max velocity setpoint increment (ramping) rate. Set to 0 to disable.



controller.current.Iq_setpoint
-------------------------------------------------------------------

ID: 30

Type: float

Units: ampere

The Iq setpoint in the user reference frame.



controller.current.Id_setpoint
-------------------------------------------------------------------

ID: 31

Type: float

Units: ampere

The Id setpoint in the user reference frame.



controller.current.Iq_limit
-------------------------------------------------------------------

ID: 32

Type: float

Units: ampere

The Iq limit.



controller.current.Iq_estimate
-------------------------------------------------------------------

ID: 33

Type: float

Units: ampere

The Iq estimate in the user reference frame.



controller.current.bandwidth
-------------------------------------------------------------------

ID: 34

Type: float

Units: hertz

The current controller bandwidth.



controller.current.Iq_p_gain
-------------------------------------------------------------------

ID: 35

Type: float



The current controller proportional gain.



controller.current.max_Ibus_regen
-------------------------------------------------------------------

ID: 36

Type: float

Units: ampere

The max current allowed to be fed back to the power source before flux braking activates.



controller.current.max_Ibrake
-------------------------------------------------------------------

ID: 37

Type: float

Units: ampere

The max current allowed to be dumped to the motor windings during flux braking. Set to zero to deactivate flux braking.



controller.voltage.Vq_setpoint
-------------------------------------------------------------------

ID: 38

Type: float

Units: volt

The Vq setpoint.



calibrate() -> void
--------------------------------------------------------------------------------------------

ID: 39

Return Type: void



Calibrate the device.

idle() -> void
--------------------------------------------------------------------------------------------

ID: 40

Return Type: void



Set idle mode, disabling the driver.

position_mode() -> void
--------------------------------------------------------------------------------------------

ID: 41

Return Type: void



Set position control mode.

velocity_mode() -> void
--------------------------------------------------------------------------------------------

ID: 42

Return Type: void



Set velocity control mode.

current_mode() -> void
--------------------------------------------------------------------------------------------

ID: 43

Return Type: void



Set current control mode.

set_pos_vel_setpoints(float pos_setpoint, float vel_setpoint) -> float
--------------------------------------------------------------------------------------------

ID: 44

Return Type: float



Set the position and velocity setpoints in the user reference frame in one go, and retrieve the position estimate

.. _api-can-rate:

comms.can.rate
-------------------------------------------------------------------

ID: 45

Type: uint32



The baud rate of the CAN interface.



comms.can.id
-------------------------------------------------------------------

ID: 46

Type: uint32



The ID of the CAN interface.



comms.can.heartbeat
-------------------------------------------------------------------

ID: 47

Type: bool



Toggle sending of heartbeat messages.



motor.R
-------------------------------------------------------------------

ID: 48

Type: float

Units: ohm

The motor Resistance value.



motor.L
-------------------------------------------------------------------

ID: 49

Type: float

Units: henry

The motor Inductance value.



motor.pole_pairs
-------------------------------------------------------------------

ID: 50

Type: uint8



The motor pole pair count.



motor.type
-------------------------------------------------------------------

ID: 51

Type: uint8



The type of the motor. Either high current or gimbal.

Options: 

- HIGH_CURRENT

- GIMBAL

motor.calibrated
-------------------------------------------------------------------

ID: 52

Type: bool



Whether the motor has been calibrated.



motor.I_cal
-------------------------------------------------------------------

ID: 53

Type: float

Units: ampere

The calibration current.



motor.errors
-------------------------------------------------------------------

ID: 54

Type: uint8



Any motor/calibration errors, as a bitmask

Flags: 

- PHASE_RESISTANCE_OUT_OF_RANGE

- PHASE_INDUCTANCE_OUT_OF_RANGE

- INVALID_POLE_PAIRS

sensors.user_frame.position_estimate
-------------------------------------------------------------------

ID: 55

Type: float

Units: tick

The filtered position estimate in the user reference frame.



sensors.user_frame.velocity_estimate
-------------------------------------------------------------------

ID: 56

Type: float

Units: tick / second

The filtered velocity estimate in the user reference frame.



sensors.user_frame.offset
-------------------------------------------------------------------

ID: 57

Type: float

Units: tick

The user defined offset.



sensors.user_frame.multiplier
-------------------------------------------------------------------

ID: 58

Type: float



The user defined multipler.



sensors.setup.onboard.calibrated
-------------------------------------------------------------------

ID: 59

Type: bool



Whether the sensor has been calibrated.



sensors.setup.onboard.errors
-------------------------------------------------------------------

ID: 60

Type: uint8



Any sensor errors, as a bitmask

Flags: 

- CALIBRATION_FAILED

- READING_UNSTABLE

sensors.setup.external_spi.type
-------------------------------------------------------------------

ID: 61

Type: uint8



The type of external sensor type. Either MA7XX, AS5047 or AMT22.

Options: 

- MA7XX

- AS5047

- AMT22

sensors.setup.external_spi.calibrated
-------------------------------------------------------------------

ID: 62

Type: bool



Whether the sensor has been calibrated.



sensors.setup.external_spi.errors
-------------------------------------------------------------------

ID: 63

Type: uint8



Any sensor errors, as a bitmask

Flags: 

- CALIBRATION_FAILED

- READING_UNSTABLE

sensors.setup.hall.calibrated
-------------------------------------------------------------------

ID: 64

Type: bool



Whether the sensor has been calibrated.



sensors.setup.hall.errors
-------------------------------------------------------------------

ID: 65

Type: uint8



Any sensor errors, as a bitmask

Flags: 

- CALIBRATION_FAILED

- READING_UNSTABLE

sensors.select.position_sensor.connection
-------------------------------------------------------------------

ID: 66

Type: uint8



The position sensor connection. Either ONBOARD, EXTERNAL_SPI or HALL.

Options: 

- ONBOARD

- EXTERNAL_SPI

- HALL

sensors.select.position_sensor.bandwidth
-------------------------------------------------------------------

ID: 67

Type: float

Units: hertz

The position sensor observer bandwidth.



sensors.select.position_sensor.raw_angle
-------------------------------------------------------------------

ID: 68

Type: int32



The raw position sensor angle.



sensors.select.position_sensor.position_estimate
-------------------------------------------------------------------

ID: 69

Type: float

Units: tick

The filtered position estimate in the position sensor reference frame.



sensors.select.position_sensor.velocity_estimate
-------------------------------------------------------------------

ID: 70

Type: float

Units: tick / second

The filtered velocity estimate in the position sensor reference frame.



sensors.select.commutation_sensor.connection
-------------------------------------------------------------------

ID: 71

Type: uint8



The commutation sensor connection. Either ONBOARD, EXTERNAL_SPI or HALL.

Options: 

- ONBOARD

- EXTERNAL_SPI

- HALL

sensors.select.commutation_sensor.bandwidth
-------------------------------------------------------------------

ID: 72

Type: float

Units: hertz

The commutation sensor observer bandwidth.



sensors.select.commutation_sensor.raw_angle
-------------------------------------------------------------------

ID: 73

Type: int32



The raw commutation sensor angle.



sensors.select.commutation_sensor.position_estimate
-------------------------------------------------------------------

ID: 74

Type: float

Units: tick

The filtered position estimate in the commutation sensor reference frame.



sensors.select.commutation_sensor.velocity_estimate
-------------------------------------------------------------------

ID: 75

Type: float

Units: tick / second

The filtered velocity estimate in the commutation sensor reference frame.



traj_planner.max_accel
-------------------------------------------------------------------

ID: 76

Type: float

Units: tick / second

The max allowed acceleration of the generated trajectory.



traj_planner.max_decel
-------------------------------------------------------------------

ID: 77

Type: float

Units: tick / second ** 2

The max allowed deceleration of the generated trajectory.



traj_planner.max_vel
-------------------------------------------------------------------

ID: 78

Type: float

Units: tick / second

The max allowed cruise velocity of the generated trajectory.



traj_planner.t_accel
-------------------------------------------------------------------

ID: 79

Type: float

Units: second

In time mode, the acceleration time of the generated trajectory.



traj_planner.t_decel
-------------------------------------------------------------------

ID: 80

Type: float

Units: second

In time mode, the deceleration time of the generated trajectory.



traj_planner.t_total
-------------------------------------------------------------------

ID: 81

Type: float

Units: second

In time mode, the total time of the generated trajectory.



move_to(float pos_setpoint) -> void
--------------------------------------------------------------------------------------------

ID: 82

Return Type: void



Move to target position in the user reference frame respecting velocity and acceleration limits.

move_to_tlimit(float pos_setpoint) -> void
--------------------------------------------------------------------------------------------

ID: 83

Return Type: void



Move to target position in the user reference frame respecting time limits for each sector.

traj_planner.errors
-------------------------------------------------------------------

ID: 84

Type: uint8



Any errors in the trajectory planner, as a bitmask

Flags: 

- INVALID_INPUT

- VCRUISE_OVER_LIMIT

homing.velocity
-------------------------------------------------------------------

ID: 85

Type: float

Units: tick / second

The velocity at which the motor performs homing.



homing.max_homing_t
-------------------------------------------------------------------

ID: 86

Type: float

Units: second

The maximum time the motor is allowed to travel before homing times out and aborts.



homing.retract_dist
-------------------------------------------------------------------

ID: 87

Type: float

Units: tick

The retraction distance the motor travels after the endstop has been found.



homing.warnings
-------------------------------------------------------------------

ID: 88

Type: uint8



Any homing warnings, as a bitmask

Flags: 

- HOMING_TIMEOUT

homing.stall_detect.velocity
-------------------------------------------------------------------

ID: 89

Type: float

Units: tick / second

The velocity below which (and together with `stall_detect.delta_pos`) stall detection mode is triggered.



homing.stall_detect.delta_pos
-------------------------------------------------------------------

ID: 90

Type: float

Units: tick

The velocity below which (and together with `stall_detect.delta_pos`) stall detection mode is triggered.



homing.stall_detect.t
-------------------------------------------------------------------

ID: 91

Type: float

Units: second

The time to remain in stall detection mode before the motor is considered stalled.



home() -> void
--------------------------------------------------------------------------------------------

ID: 92

Return Type: void



Perform the homing operation.

watchdog.enabled
-------------------------------------------------------------------

ID: 93

Type: bool



Whether the watchdog is enabled or not.



watchdog.triggered
-------------------------------------------------------------------

ID: 94

Type: bool



Whether the watchdog has been triggered or not.



watchdog.timeout
-------------------------------------------------------------------

ID: 95

Type: float

Units: second

The watchdog timeout period.



