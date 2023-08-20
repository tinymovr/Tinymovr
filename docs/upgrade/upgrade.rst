Upgrading Firmware
##################

It is possible to upgrade the firmware in two ways: Using the bootloader through UART using the ActiveFlashLight application (Windows only), and through Eclipse using a J-Link adapter.


Upgrading using bootloader
==========================

For this method you will need a USB to UART adapter or similar device to connect to Tinymovr's UART interface, such as an FTDI interface or similar. Such devices are very common and inexpensive. Please follow the diagram below to correctly setup the UART interface.


.. warning::
   The UART port on Tinymovr is NOT 5V tolerant. Applying 5V voltage will immediately damage the onboard PAC5527 controller. Please use only 3.3V for UART communication.

.. warning::
   The UART port on Tinymovr offers a 3.3v output for driving very light loads (30mA absolute max). Tinymovr cannot be powered by this pin. In addition, most UART adapters offer 5V power, which is incompatible with Tinymovr. **In short: If in doubt, leave this pin disconnected**.

Connectivity
############

R5.2
====

.. image:: connectors_r52.png
  :width: 800
  :alt: Tinymovr R5.2 connectors and pinouts

R5.0, R5.1
==========

.. image:: connectors_r5.png
  :width: 800
  :alt: Tinymovr R5.0 and R5.1 connectors and pinouts

M5.1
====

.. image:: connectors_m5.png
  :width: 800
  :alt: Tinymovr M5 connectors and pinouts

R3.x
====

.. image:: connectors.png
   :width: 800
   :alt: Tinymovr R3.x connectors and pinouts


Firmware Update Procedure for Tinymovr
######################################

Tinymovr has streamlined the firmware update process by leveraging the native CAN bus interface. This guide will walk you through the steps to refresh your Tinymovr's firmware.

Prerequisites
=============

1. **Install the Tinymovr Package**:

   If you haven’t already, install the Tinymovr package. This package also comes with the required dependencies:

   .. code-block:: bash

      pip install tinymovr

2. **Download the Firmware**:

   Head to the `Tinymovr Releases Page <https://github.com/tinymovr/Tinymovr/releases/latest>`_ and download the precompiled binary of the firmware that matches your board's hardware revision. Firmware binaries follow the format: ``tinymovr<firmware_version>-<hardware_revision>.bin``. For instance, you might come across a file named ``tinymovr1.5.0-M5.1.bin``.

Flashing the Firmware
=====================

1. **Set Up Your CAN Interface**:

   Ensure your CAN interface is connected and configured correctly with Tinymovr.

2. **Run the Script**:

   With the firmware .bin file you downloaded, execute the firmware update script:

   .. code-block:: bash

      tinymovr_dfu --node_id=1 --bin=path/to/your/downloaded/tinymovr<version>-<revision>.bin

   Replace the placeholders in the path with the appropriate values based on where you've saved the .bin file and its name.

   Example:

   .. code-block:: bash

      tinymovr_dfu --node_id=1 --bin=~/Downloads/tinymovr1.5.0-M5.1.bin

   To forgo the automatic reset post-flash, append the ``--no-reset`` flag:

   .. code-block:: bash

      tinymovr_dfu --node_id=1 --bin=~/Downloads/tinymovr1.5.0-M5.1.bin --no-reset

3. **Follow the Script's Prompts**:

   The script initially checks if the device's current firmware matches the .bin file. If they're identical, it will bypass the flashing process. If not, it will erase the prior firmware, flash the new version, and validate the update.

4. **Concluding Steps**:

   If you skipped using the ``--no-reset`` flag, the device will reboot automatically after the update. If you used the flag, you'd need to power off and on the Tinymovr by hand.

   You're all set! Your Tinymovr is updated and primed for use with the fresh firmware.

.. note::

   Before updating, it's a wise move to backup your current firmware and settings. Always pore over any version-specific instructions or release notes accompanying fresh firmware updates to stay informed.


Upgrading using J-Link
======================

Please see :ref:`setting-up-vscode`.

.. warning::
   Depending on the hardware revision and the batch, either a vertical or a sideways JST-SH connector is used for SWD. In any case, the pin order shown in the above figures is correct. Please consider this when connecting your SWD adapter.


