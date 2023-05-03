Features
________

Trajectory Planner
##################

The trajectory planner can generate acceleration- and velocity-limited trajectories, as well as time-limited trajectories for multi-axis synchronization. The planner executes at the motor control rate for smooth trajectory interpolation.

Trapezoidal Trajectories
************************

Trapezoidal trajectories are named after the shape of the velocity profile, which is a trapezoid. As such, it comprises a constant acceleration phase, where the velocity increases linearly, a constant velocity (cruise) phase, and a constant deceleration phase, where the velocity linearly decreases. 

Acceleration and Velocity-Limited Trajectory
********************************************

The acceleration- and velocity-limited planner generates trajectories that respect constraints on acceleration cruise velocity and deceleration set by the user. The corresponding time windows for each phase are computed by the planner.

Before using the planner, the desired acceleration, deceleration and max velocity limits need to be set. This can be achieved as follows:

.. code-block:: python

    tm1.traj_planner.max_accel = {max_acceleration} # ticks/sec^2
    tm1.traj_planner.max_decel = {max_deceleration} # ticks/sec^2
    tm1.traj_planner.max_vel = {mac_velocity} # ticks/sec

Once you set the desired acceleration and deceleration parameters, they do not need to be re-set. The parameters can be saved in NVRAM using `tmx.save_config()`.

Once the parameters are set, you can execute a plan to a target position:

.. code-block:: python

    tm1.traj_planner.move_to(pos_target)

As an example, a trajectory is defined and executed as follows:

.. code-block:: python

    tm1.traj_planner.max_accel = 20000
    tm1.traj_planner.max_decel = 20000
    tm1.traj_planner.max_vel = 80000
    tm1.traj_planner.move_to(100000)

Time-Limited Trapezoidal Velocity Trajectory
********************************************

The time-limited trajectory planner generates trajectories whose acceleration, cruise and deceleration phases are defined by the user. The corresponding acceleration, velocity and deceleration values are computed by the planner. In this sense, it can be thought as the 'inverse' process of the acceleration and velocity limited planner.

Similarly to the velocity-limited planner, the plan parameters need to be set first:

.. code-block:: python

    tm1.traj_planner.t_acc = 2 # seconds
    tm1.traj_planner.t_cruise = 10 # seconds
    tm1.traj_planner.t_dec = 2 # seconds
    
Now the time-limited trajectory is executed as follows:

.. code-block:: python

    tm1.traj_planner.move_to_tlimit(100000)


Multi-axis Synchronization
********************************************

Time-limited trajectories are useful for synchronizing the acceleration, cruise and deceleration phases for multiple axes. For instance, to synchronize three axes with different setpoints, you would configure the same `t_acc`, `t_cruise`, `t_dec` values. This guarantees that the trajectory phases are synchronized. Then you would issue the `move_to_tlimit` commands to each of the three different controllers in sequence:

.. code-block:: python

    tm1.traj_planner.move_to_tlimit(100000)
    tm2.traj_planner.move_to_tlimit(5000)
    tm3.traj_planner.move_to_tlimit(200000)

This will generate one trajectory for each controller, which will start and stop at the same time. 

Homing
######

The homing feature enables sensorless homing, endstop identification and retraction. The homing feature relies on detecting the mechanical resistance when an endstop is reached. During homing the position error is continuously monitored. Upon exceeding a preset threshold for more than a preset time duration, the motor is considered stalled, and the endstop found. 

Configuration
*************

Because the homing planner relies on mechanical resistance of the structure, it is important to set up the corresponding parameters correctly, otherwise the endstop sensing performance can be compromised, and damage to the structure can occur.

There are six parameters in total that control the homing behavior:

* `tmx.homing.velocity`: The velocity at which the motor performs homing
* `tmx.homing.max_homing_t`: The maximum time the motor is allowed to travel before aborting homing
* `tmx.homing.retract_dist`: The retraction distance the motor travels after the endstop has been found
* `tmx.homing.stall_detect.velocity`: The velocity below which (and together with `stall_detect.delta_pos`) stall detection mode is triggered
* `tmx.homing.stall_detect.delta_pos`: The position error above which (and together with `stall_detect.velocity`) stall detection mode is triggered
* `tmx.homing.stall_detect.t`: The time to remain in stall detection mode before the motor is considered stalled

The torque applied while the motor is stopped, until `stall_detect.t` time passes is the maximum allowed torque, as defined in the controller settings.

Operation
*********

Following proper configuration, the homing operation is initiated as follows:

.. code-block:: python

    tm1.homing.home()

