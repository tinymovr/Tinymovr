Trajectory Planner
##################

Since firmware version 0.8.8 (studio 0.3.8), Tinymovr can use a trapezoidal trajectory planner to generate acceleration- and velocity-limited trajectories, as well as time-limited trajectories for multi-axis synchronization. The planner executes at the motor control rate, which at the moment equals the PWM rate of 20kHz.

Trapezoidal Trajectories
************************

Trapezoidal trajectories are named after the shape of the velocity profile, which is a trapezoid. As such, it comprises a constant acceleration phase, where the velocity increases linearly, a constant velocity (cruise) phase, and a constant deceleration phase, where the velocity linearly decreases. 

Acceleration and Velocity-Limited Trajectory
********************************************

The acceleration- and velocity-limited planner generates trajectories that respect constraints on acceleration cruise velocity and deceleration set by the user. The corresponding time windows for each phase are computed by the planner.

Using the Tinymovr API, an acceleration- and velocity-limited trajectory has the following signatures:

.. code-block:: python

    tm1.set_max_plan_acc_dec(max_acc, max_dec)
    tm1.plan_v_limit(pos_setpoint, max_vel)

The first function call sets the max acceleration and deceleration parameters -- it does not execute a trajectory. Once you set the desired acceleration and deceleration parameters, they do not need to be re-set -- they are stored in RAM. However, upon device restart they need to be re-set, as they are not stored in non-volatile memory. The second function executes the trajectory.

As an example, a trajectory is defined and executed as follows:

.. code-block:: python

    tm1.set_max_plan_acc_dec(100000, 100000)
    tm1.plan_v_limit(100000, 50000)

Time-Limited Trapezoidal Velocity Trajectory
********************************************

The time-limited trajectory planner generates trajectories whose acceleration, cruise and deceleration phases are defined by the user. The corresponding acceleration, velocity and deceleration values are computed by the planner. In this sense, it can be thought as the 'inverse' process of the acceleration and velocity limited planner.

Using the Tinymovr API, a time-limited trajectory has the following signature:

.. code-block:: python

    tm1.plan_t_limit(pos_setpoint, total_time, acc_percent, dec_percent)

Please note that due to CAN frame limitations, the maximum trajectory time can not be longer than 65 seconds. In addition, the "acc_percent" and "dec_percent" parameters are expressed in the 0-255 range, with 0-0% and 255-100%.

As an example, a time-limited trajectory is executed as follows:

.. code-block:: python

    tm1.plan_t_limit(100000, 3000, 50, 50)


Multi-axis Synchronization
********************************************

Time-limited trajectories are useful for synchronizing the acceleration, cruise and deceleration phases for multiple axes. For instance, to synchronize three axes with different setpoints, you would issue the following three commands to three different controllers in sequence:

.. code-block:: python

    tm1.plan_t_limit(100000, 5000, 50, 50)
    tm2.plan_t_limit(5000, 5000, 50, 50)
    tm3.plan_t_limit(200000, 5000, 50, 50)

This will generate three trajectories that will start and end at the same time. Only the first parameter (the setpoint) changes, while the other ones are the same for all trajectories. This guarantees that the trajectory phases are synchronized.