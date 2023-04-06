.. _hardware-setup:

**************
Hardware Setup
**************


Requirements
############

1. A 3-phase brushless motor (see below for supported types)
2. A means to talk CAN Bus, such as CANine or a Canable-compatible adapter.
3. A mechanical rig that ensures firm connection between the Tinymovr PCB and the brushless motor. Designs that can be 3D printed are available.

Note that the Tinymovr Servo Kit includes all of the above in a ready to use kit.


Supported Motor Types
#####################

Most three-phase pancake-style outrunners can be used with Tinymovr. While there is a lot of variation between motors of even the same size and external appearance, as a general rule-of-thumb motors ranging from 40mm outer diameter to 110mm should work fine with Tinymovr.

Additional details:

Tinymovr expects a motor with sinusoidal back-EMF. Most brushless outrunners have inusoidal back-EMF. If uncertain and you have access to an oscilloscope, you can hook up the motor phases to the scope channels and check out the back-EMF for yourself. 

Motors with trapezoidal back-EMF can also be controlled, but control will be sub-optimal. The trapezoidal back-EMF will appear as residual in the dq frame, as a result it will be much harder for the current controller to regulate phase currents. The tangible result is that the motor may exhibit increased noise and vibration while running.


Mechanical Setup
################


Mounting motor and Tinymovr
***************************

The most important aspect of a correct setup is to ensure the controller is properly positioned in relation to the motor. The center of the PCB, where the encoder is located, should lie as close to the motor rotation axis as possible. In addition, the distance from the encoder magnet to the encoder IC should be less than 2mm (less than 1mm if you are mounting the PCB packwards, i.e. the encoder IC is facing away from the magnet).

.. figure:: mount.png
  :width: 800
  :align: center
  :alt: Tinymovr and motor mechanical mounting
  :figclass: align-center

  Tinymovr and motor mechanical mounting

A `3D printable encoder magnet jig <https://github.com/yconst/Tinymovr/blob/master/hardware/misc/magnet_jig.stl>`_ is available, suitable for 6mm disc magnets and 14, 19, 25 and 30mm motor hole diameters.

For a 3D printable motor mount design, check out the `Tinymovr alpha dev kit mount <https://github.com/yconst/Tinymovr/tree/master/hardware/motor-stand>`_ (suitable for 40xx motors).

.. note::
   For safety reasons, you should always ensure the motor & controller assembly are secured to a stable surface before operation. The motor rotor may experience high acceleration that may cause damage or injury if not secured properly.

.. figure:: magnet_mount.jpg
  :width: 800
  :align: center
  :alt: Left: Magnet mount directly on shaft. Right: Magnet mount using 3d-printed holder.
  :figclass: align-center

  Left: Magnet mount directly on shaft. Right: Magnet mount using 3d-printed holder.


Magnet on the rear side of the PCB

TL;DR: It is possible to have the magnet on the rear side of the PCB, i.e. opposite of the magnet sensor IC, but the gap needs to be reduced to account for the PCB thickness. 

This has been verified by MPS in `this forum post <https://forum.monolithicpower.com/t/mounting-ma702-and-magnet-on-opposite-sides-of-pcb/1609>`_, quoted below:

  [...] this type of arrangement is possible, what really matters in the end is that there is enough magnetic field reaching the sensor.
  Of course the minimum distance is imposed by the thickness of the PCB, so it puts some constraints on the design, that you have to take into account when chosing the magnet (you can use our online simulation tool for that). But as long as the PCB is not acting as a magnetic shield (due to copper plane), then it is fine.


Mounting Tips

* Ensure the encoder magnet is firmly attached to the motor shaft, otherwise it may slip out of sync. Use strong adhesive to secure.

* Calibration needs to be performed without any loads on the motor. If the motor is coupled to a load, the encoder offset angle may not be determined correctly, leading to a sub-optimal setup.

* Adjust your termination resistor DIP switch (if needed) before putting together your actuator, to avoid needing to disassemble it for adjustment later on. See also :ref:`connecting-data`.


.. _electrical-setup:

Electrical Setup
################

Electrical setup comprises three main parts: Motor connection, data connection and power connection. Below is a diagram with the electrical connection scheme.

.. image:: connections.png
  :width: 800
  :alt: Tinymovr power and data connection diagram


Connecting Motor
################

Connect the three motor phases to the three terminals on Tinymovr. The order of connection is not important, and motor direction will be determined during motor/encoder calibration.

For Tinymovr R3.x, the motor leads can be connected by soldering on the PCB. For Tinymovr R5, the leads can be connected by soldering on the PCB, or you can solder a screw terminal block and secure the motor leads on the block.

.. _connecting-data:

Connecting Data
###############

Connect the CAN bus header to one of the two CAN sockets on the board. It is not important which one you choose. If this is a terminal node in the CAN network, you may need to use a termination resistor, as follows:

* Tinymovr R3.x: flip ONLY the DIP switch labelled "CAN 120R" to on to enable the 120Ω termination resistor. 

* Tinymovr R5.x: you will need to provide an external 120Ω termination resistor.

In small setups with few nodes and short wires, it is better to enable just a single termination resistor, either on one Tinymovr board or on the CAN adapter. In setups with many nodes and long cables, you may need to enable termination resistors in both terminal nodes.

.. warning::
   The UART pins in Tinymovr R5.1 have the silkscreen reversed. If you are planning to use UART with R5.1, consult :ref:`r51-erratum-1`. 

.. _connecting-power:

Connecting Power
################

* Tinymovr R3.x can be powered from a 12-26V (3S-6S) power source.

* Tinymovr R5.x can be powered from a 12-38V (3S-9S) power source.

With the power source off/disconnected, connect the power leads observing correct polarity. Turn on/connect the power source. Upon successful power-up, the onboard LED should light up.

.. note::
   Each Tinymovr board has a capacitance of around 500μF (R3.x) / 160μF (R5). Such capacitance can introduce significant inrush current upon power-on, especially if several boards are connected to the same power supply. To prevent damage to components from overcurrent, the use of an inrush current limiter or a current-limited power supply is advised.
