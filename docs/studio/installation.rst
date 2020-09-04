.. _studio-installation:

****************************
Tinymovr Studio Installation
****************************

Preparation
###########

If using Windows with a CANtact-compatible CAN Bus adapter, such as the one included with the Tinymovr Dev Kit, you will need to install an .inf file to allow proper device naming. You can `download the inf file here <https://canable.io/utilities/windows-driver.zip>`_. Extract the archive, right click and select Install.

Using pip
#########

This is the most straightforward method to install Tinymovr studio and have access to hardware.

.. code-block:: console

    pip install tinymovr

Using git clone
###############

First clone the Tinymovr repo to a local directory:

.. code-block:: console

    git clone https://github.com/yconst/Tinymovr

Then cd to the cloned repo directory and install in developer mode:

.. code-block:: console

    cd Tinymovr/studio/Python
    pip install -e .

Launching the command line app
##############################

.. code-block:: console

    tinymovr

Command-line options
********************

Tinymovr Studio supports the following command line options.


``--iface=<iface>``
===================

The --iface option allows you to specify a CAN interface to use.

Example:

.. code-block:: console

    tinymovr --iface=robotell

All interfaces offered by python-can are supported.

``--chan=<chan>``
=================

The --chan options specifies a channel to use, optionally together with the --iface option.

Example:

.. code-block:: console

    tinymovr --iface=robotell --chan=COM3

By default, Tinymovr Studio will use slcan as the interface, and will search for CANAble-type devices.
