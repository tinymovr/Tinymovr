Encoders
########

Overview
********

This document provides information and guides for using the various encoder types supported by Tinymovr.

Onboard Magnetic
****************

All Tinymovr controllers feature an onboard magnetic absolute angle sensor that allows high precision angle measurement for efficient commutation and high bandwidth motor control. This is enabled by default and does not require any specific setup, apart from initial encoder/motor calibration.

Hall Effect Sensor
******************

Tinymovr R5 supports external Hall effect sensors for commutation and positioning. Hall effect sensors generate a specific sequence in the 3 phase encoder signal as the rotor moves. By reading this sequence, the rotor position is determined in one of six 60 degree sectors along the electrical cycle. 

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

Configuration
-------------

As a first step you need to configure the sensor type and observer bandwidth.

.. code-block:: python

    tm1.set_encoder_config(1, 100) # encoder type, bandwidth

This sets the encoder type to Hall effect sensor, and the encoder bandwidth to 100.

The bandwidth value is configurable and depends on the dynamics that you wish to achieve with your motor. Keep in mind that high bandwidth values used with motors with fewer pole pairs will make the motors oscillate around the setpoint and have a rough tracking performance (perceivable "knocks" when the rotor moves). On the other hand, too low of a bandwidth value may cause the motor to lose tracking in highly dynamic motions. If you are certain such motions will not be possible (e.g. in heavy moving platforms) you may reduce the bandwidth to ensure smoother motion.

Next, you need to set the motor configuration:

.. code-block:: python

    tm1.set_motor_config(0, 15) # motor type, pole pairs
    tm1.reset()
    
This sets the motor type and pole pairs, and restarts Tinymovr. The board restart is mandatory, as the encoder setup is performed on board startup only. Next comes motor/encoder calibration:

.. code-block:: python

    tm1.calibrate()

After calibration finishes, you should be able to control the motor:

.. code-block:: python

    tm1.velocity_control()
    tm1.set_vel_setpoint(100)

The motor should now move at a constant velocity.