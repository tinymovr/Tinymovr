
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

    params = get_bus_config(["canine", "slcan_disco"], bitrate=1000000)
    init_router(can.Bus, params)
    tm = create_device(node_id=1)

    tm.controller.calibrate()

The above code block will instantiate a Tinymovr with CAN bus id of 1 and calibrate it. `slcan_disco` is our custom slcan plugin for python-can that allows discovery of slcan-flashed CANine devices. Following the above, you can issue commands such as:

.. code-block:: python

    tm.controller.position_mode()
    tm.controller.position.setpoint = 10000

    tm.controller.velocity_mode()
    tm.controller.velocity.setpoint = 80000

BusRouter API
#############

The `BusRouter` class in the Tinymovr library is designed to facilitate message distribution and communication on the CAN bus by distributing incoming messages based on the result of a filter callback.

API Overview
------------

.. class:: BusRouter

   Initializes and manages the routing of CAN bus messages between various clients. It integrates a simple forwarding mechanism for sending messages to simplify interfacing with CAN bus objects.

   .. method:: __init__(bus_manager, timeout, logger)

      Constructor for the BusRouter. Initializes the bus manager, timeout, and logger.

      .. note::
        This constructor should not be used directly. Instead, use the :func:`init_router` function.

      :param bus_manager: Instance of BusManager to handle low-level bus interactions.
      :param timeout: Timeout in seconds for receiving messages.
      :param logger: Logger instance for logging information.

   .. method:: run()

      Main thread function that continuously listens for incoming frames and dispatches them to the appropriate clients based on filter results.

   .. method:: add_client(filter_cb, recv_cb)

      Registers a new client to receive messages that meet specific conditions.

      :param filter_cb: Callback function that determines if the incoming frame should be passed to the client.
      :param recv_cb: Callback function that handles the received frame.

   .. method:: stop()

      Stops the router's main running thread, effectively stopping the routing of messages.

   .. method:: shutdown()

      Performs a complete shutdown by stopping the router and closing all bus manager activities.

   .. method:: send(frame)

      Sends a frame via the bus manager.

      :param frame: The CAN frame to be sent.

Bus Router Management Functions
-------------------------------

These helper functions manage the lifecycle of the `BusRouter` instance.

.. function:: init_router(bus_class, bus_params, logger, timeout=0.1)

   Initializes a bus router using a python-can bus instance. This function creates a singleton instance of `BusRouter`.

   :param bus_class: The bus class from python-can to be used.
   :param bus_params: Parameters for the bus initialization.
   :param logger: Logger instance for logging activities.
   :param timeout: Timeout in seconds for receiving messages.
   :return: An instance of `BusRouter`.

.. function:: destroy_router()

   Destroys the existing bus router and stops its associated thread.

.. function:: get_router()

   Returns the current instance of the bus router if it exists.

Deprecations
------------

The following functions are deprecated and will be removed in future versions:

.. function:: init_tee()
   
   .. deprecated:: 2.1
    Use :func:`init_router` instead.

.. function:: destroy_tee()
    
   .. deprecated:: 2.1
    Use :func:`destroy_router` instead.

.. function:: get_tee()

   .. deprecated:: 2.1
    Use :func:`get_router` instead.

Error Codes
###########

Tinymovr uses error codes to indicate faults in operation. Each component presents their own error states, which are summarized in the :ref:`api-reference`.
