*********************
Tinymovr Studio Usage
*********************

Overview
########

Tinymovr Studio is an IPython application that enables communication with multiple Tinymovr instances, allowing configuration and control. 

Discovery
#########

Tinymovr Studio uses a polling mechanism to discover Tinymovr nodes in the CAN network. Upon launching the app, the first eight nodes are scanned. The ones that are present are available through the variable handle 'tmx', where x the device index. 

Basic Commands
##############

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

Upgrading Firmware
##################

It is possible to upgrade the firmware in two ways: Using the bootloader through UART using the ActiveFlashLight application (Windows only), and through Eclipse using a J-Link adapter.

Upgrading using bootloader
**************************

For this method you will need a USB to UART adapter or similar device to connect to Tinymovr's UART interface, such as an FTDI interface or similar. Such devices are very common and inexpensive. Please follow the diagram below to correctly setup the UART interface.

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
**********************

Please see :ref:`setting-up-eclipse` and :ref:`setting-up-jlink`.


We are actively working to provide a cross-platform utility to allow easy firmware file flashing using the command line.