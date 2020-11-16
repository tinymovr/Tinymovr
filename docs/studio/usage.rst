*********************
Tinymovr Studio Usage
*********************

Overview
########

Tinymovr Studio is an IPython application that enables communication with multiple Tinymovr instances, allowing configuration and control. 


Launching the command line app
##############################

.. code-block:: console

    tinymovr


Discovery
#########

Tinymovr Studio uses a polling mechanism to discover Tinymovr nodes in the CAN network. Upon launching the app, by default the first ten nodes are scanned. The ones that are present are available through the variable handle 'tmx', where x the device index. To specify the scan range, take a look at :ref:`command-line-options`.


Issuing Commands
################

You can read variables and issue commands using the respective Tinymovr handle, e.g.:

.. code-block:: python

    tmx.device_info

or

.. code-block:: python

    tmx.set_vel_setpoint(0)

Where x is the device ID. Full tab completion is available.


Multiple Instances
##################

In order for multiple Tinymovr instances to coexist in the same network, they need to have unique IDs. The default ID is 1. To assign different IDs to each board, follow the method below:

1. Connect a single Tinymovr to the bus and launch Studio. The board will be assigned the default ID, 1, and will be accessible as tm1.

2. Issue the id change

.. code-block:: python

    tm1.set_can_config(x)

where x is the desired ID. Valid IDs are from 1-64, but the Studio app currently discovers IDs up to 8.

3. Relaunch Studio

4. The board will be discovered with the new ID. Save configuration.

.. code-block:: python

    tmx.save_config()

5. Power down or reset the board. Tinymovr is now ready to use with the new ID.

.. _command-line-options:

Command-line options
####################

Tinymovr Studio supports the following command line options.


``--ids=<ids>``
===============

The --ids option specifies a set of CAN node IDs to scan. 

Example:

.. code-block:: console

    tinymovr --ids=1,3,5,7-9

All syntax options supported by `Pynumparser <https://pypi.org/project/pynumparser/>`_ are available.


``--bustype=<bustype>``
=======================

The --bustype option specifies a CAN bus type to use.

Example:

.. code-block:: console

    tinymovr --bustype=robotell

All interfaces offered by python-can are supported.


``--chan=<chan>``
=================

The --chan options specifies a channel to use, optionally together with the --bustype option. 

Example:

.. code-block:: console

    tinymovr --bustype=robotell --chan=COM3

By default, Tinymovr Studio will use slcan as the interface, and will search for CANAble/CANtact-type devices with slcan firmware. Such is the CANine adapter supplied with Tinymovr Dev Kits.


Units
#####

Tinymovr Studio introduced physical units and quantities since v0.3.0. Units are introduced through the `Pint <https://pypi.org/project/Pint/>`_ package. Using units you will see all physical quantities that you query associated with a unit. For instance, you can do the following:

.. code-block:: python

    In [1]: tm1.encoder_estimates
    Out[1]: {'position': 0.0 <Unit('tick')>, 'velocity': 0.0 <Unit('tick / second')>}

You can also set quantities in any (defined) unit you wish. For instance:

.. code-block:: python

    In [1]: tm1.set_pos_setpoint(3.0 * ureg('rad'))

The above will set the rotor position to 3 radians from the initial position. Similarly for velocity:

.. code-block:: python

    In [1]: tm1.set_pos_setpoint(3.0 * ureg('rad/s'))

Will set velocity to 3 radians/second. If not unit is used in setting a value, the default units will be assumed, in the above cases ticks and ticks/second.

The ureg object is the unit registry, and it is that which holds all unit definitions. You can use it to do all sorts of cool stuff such as doing conversions or defining your own units.

For more information on units and their usage, take a look at `Pint's documentation <https://pint.readthedocs.io/en/stable/>`_


Socketcan & Linux
#################

You can use a socketcan-enabled CAN adapter with Tinymovr Studio. The CANine adapter supplied with Tinymovr Dev Kits supports Socketcan natively with the alternative Candlelight Firmware. To connect to a Socketcan device, run Studio as follows:

.. code-block:: console

    tinymovr --bustype=socketcan --chan=CAN0


Tinymovr in-silico
##################

Tinymovr studio implements a simplistic simulation of the actual controller, in order to facilitate validation of basic commands etc. To use the simulation mode, run Studio as follows:

.. code-block:: console

    tinymovr --bustype=insilico --chan=test

Basic commands such as :code:`state`, :code:`encoder_estimates`, :code:`set_pos_setpoint` work, more to be implemented soon.


Upgrading Firmware
##################

It is possible to upgrade the firmware in two ways: Using the bootloader through UART using the ActiveFlashLight application (Windows only), and through Eclipse using a J-Link adapter.


Upgrading using bootloader
==========================

For this method you will need a USB to UART adapter or similar device to connect to Tinymovr's UART interface, such as an FTDI interface or similar. Such devices are very common and inexpensive. Please follow the diagram below to correctly setup the UART interface.


.. warning::
   The UART port on Tinymovr is NOT 5V tolerant. Applying 5V voltage will immediately damage the onboard PAC5527 controller. Please use only 3.3V for UART communication.
   
(image of UART ports)

Qorvo provides an application to interface with the bootloader environment using UART and enable firmware upgrades. It is available through the `Qorvo website <https://www.qorvo.com/products/p/PAC5527#evaluation-tools>`_ as an archive named "PAC55xx ActiveFlashLight Bootloader". Please download and extract the archive and follow these steps:

1. Inside the "resources" folder run the ActiveFlashLight application
2. Power on Tinymovr
3. Under 'COMMUNICATION' click on 'Connect'
4. Power off and on Tinymov. The application status should now indicate that it is connected to the device. 
5. Under 'PROGRAMMING' click on 'Browse...' and find the firmware .bin file
6. Under 'PROGRAMMING' click on 'Auto Program'. The application will erase, flash and verify the device with the new firmware. Wait for the process to complete
7. Once complete, power off Tinymovr and quit the application

You should now be able to use Tinymovr as usual with the new firmware.


Upgrading using J-Link
======================

Please see :ref:`setting-up-eclipse` and :ref:`setting-up-jlink`.


We are actively working to provide a cross-platform utility to allow easy firmware file flashing using the command line.
