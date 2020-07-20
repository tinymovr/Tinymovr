**************
Hardware Guide
**************

.. note::
   Images in the documentation may refer to different hardware versions. Where incompatibilities among hardware versions exist, this is noted in the image caption.


Overview
########

Tinymovr is a brushless motor controller in the form of a small PCB that can be integrated to the back of a motor. Tinymovr uses Field Oriented Control for efficient commutation of motor phases, together with position feedback from an integrated magnetic encoder. 


Supported Motor Types
#####################

Most three-phase pancake-style outrunners can be used with Tinymovr. While there is a lot of variation between motors of even the same size and external appearance, as a general rule-of-thumb motors ranging from 40mm outer diameter to 110mm should work fine with Tinymovr.

(image)

Mechanical Setup
################

The most important aspect of a correct setup is to ensure the controller is properly positioned in relation to the motor. The center of the PCB, where the encoder is located, should lie as close to the motor rotation axis as possible. In addition, the distance from the encoder magnet to the encoder IC should be less than 2mm (less than 1mm if the magnet is on the back side of the PCB).

(image)

A series of motor, PCB and magnet mount designs are available to 3D print for various kinds of motor hole patterns.

.. note::
   For safety reasons, you should always ensure the motor&controller assembly are securely fastened to a stable surface before operation. The motor may experience high acceleration that may cause injury or damage.

Electrical Setup
################

Electrical setup is simple, just connect the three motor phases to the three terminals on Tinymovr. The order of connection is not important and will be determined during motor/encoder calibration.

(image)

