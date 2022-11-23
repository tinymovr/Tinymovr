************
Studio Usage
************

Overview
########

Tinymovr Studio is a cross-platform GUI application, CLI application, and Python library that offers easy access to all of Tinymovr's functionality.


Launching the GUI app
#####################

.. code-block:: console

    tinymovr


Launching the CLI app
#####################

.. code-block:: console

    tinymovr_cli

Alternative Adapters/Firmwares
##############################

By default Tinymovr Studio searches for slcan-compatible CAN bus adapters, which appear as USB Virtual COM Port devices. To specify an alternative device, use the `--bustype` and `--chan` command line arguments. 

For instance, to work with adapters using the `CANine firmware <https://github.com/tinymovr/CANine>`_, launch Tinymovr Studio as follows:

.. code-block:: console

    tinymovr --bustype=canine —chan=0xC1B0

Note that you need to have completed `setting up CANine <https://canine.readthedocs.io/en/latest/canine.html#canine-firmware>`_ before issuing the above command to work with CANine.

Compatibility
#############

Tinymovr Studio includes by default a checksum comparison to determine protocol compatibility between firmware and studio version. This is performed each time a node is discovered, and prior to initializing the tinymovr object. If you see a compatibility-related message, please upgrade to the latest studio and firmware versions. 


Issuing Commands
################

You can read/write variables and issue commands using the respective Tinymovr handle, e.g.:

.. code-block:: python

    tm1.encoder

or

.. code-block:: python

    tm1.controller.pos_setpoint = 10000

Replace "tm1" with the correct device ID if necessary. Full tab completion is available.


Multiple Instances
##################

In order for multiple Tinymovr instances to coexist in the same CAN network, they need to have unique IDs. The default ID is 1. To assign different IDs to each board, follow the method below:

1. Connect a single Tinymovr to the bus and launch Studio. The board will be assigned the default ID, 1, and will be accessible as tm1.

2. Issue the id change

.. code-block:: python

    tm1.comms.can.id = x

where x is the desired ID. You can assign IDs in the range 1-1024.

3. Relaunch Studio

4. The board will be discovered with the new ID. Save configuration.

.. code-block:: python

    tm1.save_config()

5. Power down or reset the board. Tinymovr is now ready to use with the new ID.

.. _command-line-options:

Command-line options
####################

Tinymovr Studio supports the following command line options.


``--bus=<bus>``
=======================

The --bus option specifies a CAN bus type to use.

Example:

.. code-block:: console

    tinymovr --bus=socketcan

All interfaces offered by python-can are supported.


``--chan=<chan>``
=================

The --chan options specifies a channel to use, optionally together with the --bustype option. 

Example:

.. code-block:: console

    tinymovr --bustype=socketcan --chan=CAN0

By default, Tinymovr Studio will use slcan as the interface, and will search for CANAble/CANtact-type devices with slcan firmware. Such is the CANine adapter supplied with Tinymovr Servo Kits.


Units
#####

Tinymovr Studio introduced physical units and quantities since v0.3.0. Units are introduced through the `Pint <https://pypi.org/project/Pint/>`_ package. Using units you will see all values that you query associated with a unit, which forms a physical quantity.

With units, you can do the following:

.. code-block:: python

    In [1]: tm1.encoder.pos_estimate
    Out[1]: 0.0 <Unit('tick')>

You can also set quantities in any (defined) unit you wish. For instance:

.. code-block:: python

    In [1]: tm1.controller.pos_setpoint = 2.0 * rad

The above will set the rotor position to 2 radians from the initial position. Similarly for velocity:

.. code-block:: python

    In [1]: tm1.controller.vel_setpoint = 3.0 * rad / second

Will set velocity to 3 radians/second. If not unit is used in setting a value, the default units will be assumed, in the above cases ticks and ticks/second.

The ureg object is the unit registry, and it is that which holds all unit definitions. You can use it to do all sorts of cool stuff such as doing conversions, defining your own shortcuts or even new units.

For instance, to define a few frequently used shortcuts in a program:

.. code-block:: python

    from avlos import get_registry
    ureg = get_registry()
    mA = ureg.milliampere
    rad = ureg.radian
    s = ureg.second

Then you can use the defined shortcuts to intuitively set values, such as a position setpoint with velocity and current feed-forwards:

.. code-block:: python

    tm1.controller.pos_setpoint = 2*PI * rad
    tm1.controller.vel_setpoint = PI * rad/second
    tm1.controller.cur_setpoint = 1.5 * ampere

Take a look at the :ref:`api-reference` for default units used in each command.

For more information on units and their usage, take a look at `Pint's documentation <https://pint.readthedocs.io/en/stable/>`_


Socketcan & Linux
#################

You can use a socketcan-enabled CAN adapter with Tinymovr Studio. The CANine adapter supplied with Tinymovr Servo Kits supports Socketcan natively with the alternative Candlelight Firmware. To connect to a Socketcan device, run Studio as follows:

.. code-block:: console

    tinymovr --bustype=socketcan --chan=CAN0

