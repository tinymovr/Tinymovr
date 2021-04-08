.. _Control theory:

********************
Control loop theory
********************

Field Oriented Control
######################

Also know as FOC, this is an optimal control scheme used with brushless motors. It is more complex than other algorithms and requires more computing power. A 32 bit micro controller is now enough to run it so it is becoming more and more common. It allows for greater performance and controlling directly the current in the phases and consequently the output torque of the motor.

`FOC in details <https://en.wikipedia.org/wiki/Vector_control_(motor)>`_

Measuring phase currents
************************

What is important to understand is that in order to work, the algorithm must measure precisely the currents in the 3 phases (a,b,c) of the motor. It is done with the help of the shunt resistors.

Motor electrical angle
**********************

It also needs to know the electrical angle between the stator and the rotor. In Tinymovr, it is computed from the magnetic encoder. This is called a *sensored foc*, other implementation use some mathematical estimation based on the back-emf of the motor, and can work even at high velocities, they are called *sensorless*.

Current target
***************

The input of the algorithm is the value of the target current in the quadrature frame, which is generally called **Iq**.

Calibration procedure
*********************

Because of this complexity, the FOC loop depends on some motor parameters, hence a calibration procedure is needed to identify them.

All of this is tuned automatically by the firmware during the calibration procedure. So you don't need to worry about it.


High Level Control loop
#######################

On top of this FOC loop, Tinymovr implements an embedded control loop with different modes and setpoints. Since it is embedded, it runs much faster at a 20KHz sample rate, than an external loop could do when communicating via the CAN bus.

Position mode
*************

This is the most versatile mode, it accepts a Target Position, and additional velocity and current feedforward parameters.

.. image:: control_loop.png
  :width: 800
  :alt: Tinymovr dimensions

You can send these 3 setpoints as a single CAN frame when using the `set_pos_setpoint` endpoint.

You can tune separately each gain of the loop.

**P**: *Stiffness*
    The gain of the position loop.
**V**: *Damping*
    The gain of the velocity loop.
**I**: Velocity Integrator gain
    Especially useful for tracking positions at low velocities.
    You can set it to zero for greater position control bandwith.

Example applications
--------------------

Robot joint control, CNC axis.

Velocity mode
*************

In this mode you can follow a target velocity.

Example applications
--------------------

Drone propeller
    It won't go to high velocities though because of the sampling rate of the encoder. You should use classic ESC's for this, or a sensorless FOC board.
Wheel propulsion
    For brushless wheel based projects like differential drives or rovers.

Current mode
*************

This is the most direct mode, you can send targets directly to the FOC algorithm.

Example applications
--------------------

Force based control
    Robots controlled in admittance or impedance.
E-scooter
    Where the throttle input is mapped to the current target. It translates to the acceleration of the vehicle, which feels more natural than a velocity input.
