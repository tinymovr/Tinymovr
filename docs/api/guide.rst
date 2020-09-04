*********
API Guide
*********

Overview
########

The Tinymovr API allows full hardware control from within Python scripts, using a hig-level interface to hardware.

Installation
############

Tinymovr API is part of Tinymovr Studio. For help installing Studio, please take a look at :ref:`studio-installation`.

Use from a Python Script
########################

Here below is an example using the API from Python scripts and controlling hardware:

.. code-block:: python

    import can
    from tinymovr import Tinymovr

    bus = can.Bus(bustype="cantact", channel="COM1, bitrate=1000000)
    iface = CAN(bus)
    tm = Tinymovr(node_id=1, iface=iface)

    tm.calibrate()

Teh above code block will instantiate a Tinymovr with CAN bus id of 1 and calibrate it. Following the above, you can issue commands such as:

.. code-block:: python

    tm.position_control()
    tm.set_pos_setpoint(0)

    tm.velocity_control()
    tm.set_vel_setpoint(80000)

API Command Reference
#####################

Please take a look at :ref:`command-reference`.
