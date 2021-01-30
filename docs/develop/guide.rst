******************
Firmware Dev Guide
******************


Overview
########

This document provides a guide for setting up a development environment for developing Tinymovr firmware. Development is currently possible in the Qorvo-supplied Eclipse distribution in Windows. Any help for porting support for development to Macos and Linux would be greatly appreciated!

It is possible to build the Tinymovr firmware using make and gcc, however this configuration is outdated and should be avoided. In particular, this generated a non-bootloader enabled binary and may brick your board.


Hardware Connections
####################

For proper debuggin capabilities you will need a Segger J-Link adapter. Unfortunately original J-Link adapters are quite expensive. A more affordable option is the J-Link EDU adapter at around $60 or the J-Link EDU mini adapter at around $20. In addition, there are J-Link clones that can be purchased for very low prices on ebay or Aliexpress. However, reliability of these clones is not guaranteed.

With the board and J-Link adapter powered off, connect the J-Link to Tinymovr as shown below:

(image)

Setting up the repo
###################

First, clone the Tinymovr repo:

.. code-block:: console

    git clone https://github.com/yconst/Tinymovr

The Tinymovr repo includes the firmware source code and supporting files, however the PAC55xx SDK is not included due to licensing restrictions imposed by Qorvo. Thus, you will need to `download it from the Qorvo website <https://www.qorvo.com/products/p/PAC5527#evaluation-tools>`_, where you will need to supply your email.

The file comes in a zipped installer exe (!), which all it does is extract the contents to a directory. Navigate to the extracted files directory and copy the 'pac55xx_sdk' directory inside the Tinymovr repo:

.. code-block:: console

    cp -r <pac55xx_sdk_location> <tinymovr_location>/firmware/

Now you have the required PAC SDK almost ready. As a final step, there is a small patch that you will need to apply in the pac55xx_sdk directory. It is suggested to use the `Python patch package <https://pypi.org/project/patch/>`_, which is cross-platform. If you do not have the package, install using pip:

.. code-block:: console

    pip3 install patch

Then:

.. code-block:: console

    cd <tinymovr_location>/firmware/pac55xx_sdk
    python3 -m patch patch.patch

Done.

.. _setting-up-eclipse:

Setting up Eclipse
##################

You will need the Qorvo-supplied Eclipse distribution. Download from the `Qorvo website <https://www.qorvo.com/products/p/PAC5527#evaluation-tools>`_.

Run the downloaded installer that will install Eclipse and the J-Link utility (ver. 6.31) for you.

Once installation is complete, run Eclipse and import the Tinymovr project. Try building to verify everything is ok.

.. _setting-up-jlink:

Setting up J-Link
#################

The required J-Link drivers and software, together with instructions, can be found in the `Qorvo website <https://www.qorvo.com/products/p/PAC5527#evaluation-tools>`_, under the download 'Segger J-Link Support'. This download includes a nacessary patch to enable J-Link to work with Qorvo devices. Instructions on how to apply the patch are included.

Once you have the J-Link adapter wired up and the software installed, you are ready to flash and debug the firmware in Eclipse. Please note that the default configuration in Eclipse is configured to flash the bootloader and firmware together.

TODO: Writeup debugging bootloader.
