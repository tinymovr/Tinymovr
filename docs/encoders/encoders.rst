Sensors and Encoders
####################

Overview
********

This document provides information and guides for using the various angle sensor and encoder types supported by Tinymovr.

Observer bandwidth
******************

Tinymovr uses an observer in order to filter readings from the sensors. The bandwidth value corresponds to the desired observer bandwidth. It is a configurable value and depends on the dynamics that you wish to achieve with your motor. Keep in mind that high bandwidth values used with motors with fewer pole pairs will make the motors oscillate around the setpoint and have a rough tracking performance (perceivable "knocks" when the rotor moves). On the other hand, too low of a bandwidth value may cause the motor to lose tracking in highly dynamic motions. If you are certain such motions will not be possible (e.g. in heavy moving platforms) you may reduce the bandwidth to ensure smoother motion.

Onboard Magnetic
****************

All Tinymovr controllers feature an onboard magnetic absolute angle sensor that allows high precision angle measurement for efficient commutation and high bandwidth motor control. This is enabled by default and does not require any specific setup, apart from initial angle sensor/motor calibration.

The onboard angle sensor is enabled by default, so no special configuration is necessary. Should you need to switch to the onboard sensor, use the following commands:

.. code-block:: python

    tmx.encoder.type = 0
    tmx.encoder.bandwidth = 300
    tmx.save_config()
    tmx.reset() # sensor type change is applied after reset

The value of 300Hz in bandwidth is the default value configured based on the charasteristics of the onboard angle sensor.

Hall Effect Sensor
******************

Tinymovr R5 supports external Hall effect sensors for commutation and positioning. Hall effect sensors generate a specific sequence in the 3 phase Hall effect sensor signal as the rotor moves. By reading this sequence, the rotor position is determined in one of six 60 degree sectors along the electrical cycle. 

Hardware Setup
--------------

To use Hall effect sensors, you need to connect the sensor's power supply, phases and ground to the correct pins on the Tinymovr board. The pinout for the Hall effect sensor connector is shown below.

.. image:: hall_pinout.jpg
  :width: 800
  :alt: Hall effect sensor connection diagram

.. note::
  The diagram shows the connector side of the board, i.e. the side where the CAN, UART and SPI connectors, and also the DC-link capacitors are located.

Note the U, V and W pins. These need to be connected to the respective pins of the sensor. The pin labeled T is currently not in use. In addition, the 3.3V power supply and the GND need to be connected to the sensor as well.

.. note::
  Tinymovr supplies 3.3V on the AUX power supply pin. If your sensor uses 5V, or if it needs more than 50mA, you'll need to provide power externally, e.g. through a dedicated buck converter. 

Example
=======

The figure below shows an example of wiring a hub motor to Tinymovr, using the embedded Hall effect sensors of the motor for commutation. Note that the power and ground are connected to an external 5V power supply. Alternatively, if your Hall effect sensors are compatible with 3.3V input, you can connect the leads to the onboard connectors.

.. image:: hubmotor_diagram.png
  :width: 800
  :alt: Wiring diagram for connection of hub motor to Tinymovr

Configuration
-------------

As a first step you need to configure the sensor type and observer bandwidth.

.. code-block:: python

    tmx.encoder.type = 1
    tmx.encoder.bandwidth = 100

This sets the type to Hall effect sensor, and the observer bandwidth to 100Hz. Note that the change of the sensor type will not propagate until the next reset; in effect, the `encoder.type` variable value will still show as the previous one, here as 0, until you finish configuration and reset.

Next, you need to set the motor pole pairs:

.. code-block:: python

    tmx.motor.pole_pairs = 15
    
As a final step, save configuration and reset:

.. code-block:: python

    tmx.save_config()
    tmx.reset() # sensor type change is applied after reset

The board needs to be reset following saving of the config, to enable the sensor type change. For safety reasons, any change to the sensor type is only enabled at next boot. 

Next comes tuning of gains. Gains are determined on the resolution of a full mechanical turn fo the motor. When using the onboard magnetic sensor, the resolution is fixed to 8192 ticks. However, when using the Hall effect sensor, the mechanical resolution is variable, and amounts to `6 * pole_pair_count`. As such, if you have a 15 pp motor, your mechanical resolution would be 90. 

Because of this vast change in resolution (almost 2 orders of magnitude), the gains need to be updated:

.. code-block:: python

    tmx.controller.position.p_gain = 5
    tmx.controller.velocity.p_gain = 0.07

The values above are just an example using a 15 pp hoverboard motor. For your own motor, you need to determine these experimentally. In position control mode, start by raising the default velocity gain until your motor experiences oscillations. The back up by a factor of two, and repeat the same for position control. This simple tuning heuristic does not result in an optimal configuration but the gains are workable.

At this point, you are ready to perform motor/sensor calibration. This will measure the R and L values of the motor, as well as the hall effect sensor sequence.

.. code-block:: python

    tmx.calibrate()

After calibration finishes, you should be able to control the motor:

.. code-block:: python

    tmx.controller.velocity_mode()
    tmx.controller.velocity.setpoint = 100 # around 60 rpm for a 15 pp motor

The motor should now move at a constant velocity.

Once you have determined that the motor behaves as expected, set to idle and perform another config save to persist the configuration:

.. code-block:: python

    tmx.idle()
    tmx.save_config()

