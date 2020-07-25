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

Flashing
########