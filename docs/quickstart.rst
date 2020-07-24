*****************
From Zero to Spin
*****************

Preparation
###########

If you are using a Tinymovr Dev Kit you can continue with the steps outlined in this document.

If you have a Tinymovr board, please take a look at the Hardware Setup Guide first to complete hardware setup.

Before proceeding to the next steps, ensure your Tinymovr is powered up.

Installing and Launching Studio
###############################

It is advised to use an Anaconda environment with Python >=3.7. You can check you Python version as follows:

.. code-block:: console

    python --version

Should output something like:

.. code-block:: console

    Python 3.7.6

Once you have your environment ready, issue the following:

.. code-block:: console

    pip install tinymovr
    tinymovr

You should now be inside the Tinymovr Studio IPython interface.

Checking Functionality and Calibrating
######################################

.. code-block:: python

    tm1.device_info

IPython should display an array of device-related information.

If you are using a Tinymovr Dev Kit, the motor and encoder should have been already calibrated. Otherwise, you'll have to follow the below short calibration procedure.

.. note::
   After issuing the command below, the motor will spin. Ensure the rotor is free of obstructions or loads.

.. code-block:: python

    tm1.calibrate()

Your Tinymovr is now ready for operation. You can view information about the attached motor as follows:

.. code-block:: python

    tm1.motor_info

Testing Position Control Mode
#############################

.. note::
   After issuing the command below, the motor will hold position and may spin.

.. code-block:: python

    tm1.set_position_control()

The motor should now be actively holding it's position. Try moving it by hand and you should feel resistance.

Now try to command a new position:

.. code-block:: python

    tm1.set_pos_setpoint(0)