
.. _integrating:

Integrating Tinymovr
++++++++++++++++++++

Tinymovr can be integrated into your projects in the form of a Python library. Using the library, you can generate and manipulate one or more Python objects that represent Tinymovr controllers, with the full device tree, as exposed in the Studio GUI and CLI. The communication protocol defined by Avlos and used by Tinymovr comprises a series of read/write endpoints. The endpoints are defined taking into account the capabilities and constraints of the CAN bus, the main communication bus used by Tinymovr, however they are not strictly constrained to CAN, and can be used with other comm channels as well.

The Tinymovr Library allows full hardware control from within Python scripts, using a high-level interface to hardware. For use with embedded devices, the Tinymovr C++ Library is proposed as an alternative.

It is of course possible to interface directly with Tinymovr using the protocol endpoints by generating the message frames yourself. However, this is generally advised against, as the libraries offer robustness and functionality that is not trivial to implement from scratch.

The Tinymovr Python library is part of Tinymovr Studio. For help installing Studio, please take a look at :ref:`studio-installation`.

For the full Tinymovr protocol reference, take a look at the :ref:`api-reference`

Use with Python
###############

Here below is an example using the API from Python scripts and controlling hardware:

.. code-block:: python

    from tinymovr.config import get_bus_config, create_device

    bus = get_bus_config(["socketcan"])
    tm = create_device(node_id=1, bus=bus)

    tm.calibrate()

The above code block will instantiate a Tinymovr with CAN bus id of 1 and calibrate it. Following the above, you can issue commands such as:

.. code-block:: python

    tm.controller.position_mode()
    tm.controller.pos_setpoint = 10000

    tm.controller.velocity_mode()
    tm.controller.vel_setpoint = 80000

Error Codes
###########

Tinymovr uses error codes to indicate faults in operation. These are listed below. Note that using Tinymovr studio, the error codes are already presented with an explanation.

0: ``NO_ERROR``
***************

No error present.

1: ``INVALID_STATE``
********************

An invalid state has been requested. This can be triggered when attempting to transition to a state whose controller state constraints are not satisfied. E.g. switching to closed loop control without calibrating.

2: ``ILLEGAL_VALUE``
********************

This is a legacy error code that is not in use.

3: ``VBUS_UNDERVOLTAGE``
************************

The bus voltage has dropped below the undervoltage threshold. In a current-limited power supply, this may also indicate excessive current demand from the power supply.

4: ``OVERCURRENT``
******************

The phase current has exceeded the overcurrent threshold. The overcurrent threshold is 1,5 times the user-defined current limit, and in any case no more than 50A.

5: ``PWM_LIMIT_EXCEEDED``
*************************

This is a legacy error code that is not in use.

6: ``PHASE_RESISTANCE_OUT_OF_RANGE``
************************************

The phase resistance measured during calibration is out of range. The defined range is 5mΩ to 1Ω.

7: ``PHASE_INDUCTANCE_OUT_OF_RANGE``
************************************

The phase inductance measured during calibration is out of range. The defined range is 2μH to 5mH.

8: ``INVALID_POLE_PAIRS``
*************************

The pole pair detection algorithm did not converge near an integer number during calibration.

9: ``ENCODER_READING_UNSTABLE``
*******************************

Encoder reading variation is over maximum allowed threshold. This is usually the casse if the magnet is misaligned, too far away from the encoder IC, or missing.


