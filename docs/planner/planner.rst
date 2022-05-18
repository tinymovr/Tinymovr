Trajectory Planner
##################

Since firmware version 0.8.8 (studio 0.3.8), Tinymovr can use a trapezoidal trajectory planner to generate acceleration- and velocity-limited trajectories, as well as time-limited trajectories for multi-axis synchronization. The planner executes at the motor control rate, which at the moment equals the PWM rate of 20kHz.

Trapezoidal Trajectories
************************

Trapezoidal trajectories are named after the shape of the velocity profile, which is a trapezoid. As such, it comprises a constant acceleration phase, where the velocity increases linearly, a constant velocity (cruise) phase, and a constant deceleration phase, where the velocity linearly decreases. 

Acceleration and Velocity-Limited Trajectory
********************************************

The acceleration- and velocity-limited planner generates trajectories that respect constraints on acceleration cruise velocity and deceleration set by the user. The corresponding time windows for each phase are computed by the planner.

Before using the planner, the desired acceleration, deceleration and max velocity limits need to be set. This can be achieved as follows:

.. code-block:: python

    tm1.traj_planner.max_acc = {max_acceleration} # ticks/sec^2
    tm1.traj_planner.max_dec = {max_deceleration} # ticks/sec^2
    tm1.traj_planner.max_vel = {mac_velocity} # ticks/sec

Once you set the desired acceleration and deceleration parameters, they do not need to be re-set -- they are stored in RAM. However, upon device restart they need to be re-set, as they are not stored in non-volatile memory. Once the parameters are set, you can execute a plan to a target position:

.. code-block:: python

    tm1.traj_planner.move_to(pos_target)

As an example, a trajectory is defined and executed as follows:

.. code-block:: python

    tm1.traj_planner.max_acc = 20000
    tm1.traj_planner.max_dec = 20000
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

This will generate three trajectories that will start and end at the same time (with 1-2 milliseconds delay, due to sequential transmission of commands). 