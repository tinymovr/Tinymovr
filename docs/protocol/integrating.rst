
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

    import can
    from tinymovr.bus_router import init_router
    from tinymovr.config import get_bus_config, create_device

    params = get_bus_config(["canine", "slcan_disco"])
    params["bitrate"] = bitrate
    init_router(can.Bus(**params))
    tm = create_device(node_id=1)

    tm.controller.calibrate()

The above code block will instantiate a Tinymovr with CAN bus id of 1 and calibrate it. `slcan_disco` is our custom slcan plugin for python-can that allows discovery of slcan-flashed CANine devices. Following the above, you can issue commands such as:

.. code-block:: python

    tm.controller.position_mode()
    tm.controller.position.setpoint = 10000

    tm.controller.velocity_mode()
    tm.controller.velocity.setpoint = 80000

Error Codes
###########

Tinymovr uses error codes to indicate faults in operation. Each component presents their own error states, which are summarized in the :ref:`api-reference`.
