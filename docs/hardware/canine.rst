
**********************
CANine adapter
**********************

Introduction
############

CANine is a USB Type-C <-> CAN Bus adapter. Itself it is an adaptation of the CANable board. It features DF-13 connectors in addition to 2.54mm header, as well as a convenient DIP switch for setting CAN termination resistor and boot mode.

This design is Open Source, released under the original CANable license.

Hardware
########

* STM32F042C6 MCU
* MCP2551 CAN Bus Transceiver
* Dual DF13/2.54mm Pin header CAN interface
* USB Type-C (v2) / micro (v1) interface

Hardware Configuration
######################

.. figure:: canine_config.png
  :width: 800
  :align: center
  :alt: CANine hardware configuration diagram
  :figclass: align-center

CAN Termination Resistor
########################

CANine includes a termination resistor to ease the setup of a CAN Bus network. If CANine is a terminal node in the network, you should enable the 120Ohm termination resistor via the onboard DIP switch. In small setups with a few nodes and short wires, it is enough to enable just a single termination resistor, either on the CANine or on a single Tinymovr.

Flashing Firmware
#################

Your CANine adapter comes pre-flashed with the slcan firmware. As such you do not need to perform flashing to use it together with Tinymovr Studio on Windows, Macos or Ubuntu Linux.

If you would like to upgrade the firmware, the easiest solution is to use the `canable updater <https://canable.io/updater/>`_. It is a DFU updater working directly from within the browser. Chrome and Edge are supported.

With the board powered off, move the BOOT DIP switch to the "ON" position. Connect your board and follow the instruction on the web page to begin the flash procedure.


Use in Windows
##############

If using Windows with a CANtact-compatible CAN Bus adapter, such as CANine, you will need to install an .inf file to enable proper device naming. You can `download the inf file here <https://canable.io/utilities/windows-driver.zip>`_. Extract the archive, right click and select Install.
