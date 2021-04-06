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