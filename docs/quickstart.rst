*****************
From Zero to Spin
*****************

Preparation
###########

If you are using a Tinymovr Dev Kit, please ensure you have completed :ref:`connecting-data` and :ref:`connecting-power`.

If you are using a Tinymovr board in your own setup, please go through the :ref:`hardware-setup-guide`.

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

You should now be looking at the Tinymovr Studio IPython interface.

Checking Functionality and Calibrating
######################################

.. code-block:: python

    tm1.device_info

IPython should display an array of device-related information.

If you are using a Tinymovr Dev Kit, the motor and encoder should have been already calibrated. If you have your own setup, or if you experience problems with prior calibration, you'll need to go through the following brief calibration procedure.

.. note::
   After issuing the command below, the motor will spin. Ensure the rotor is free of obstructions or loads.

.. code-block:: python

    tm1.calibrate()

Follow the on-screen prompts. The motor will produce an audible beep and rotate in one direction.
Your Tinymovr is now ready for operation. You can view information about the attached motor as follows:

.. code-block:: python

    tm1.motor_info

This will reveal identified motor parameters, namely: phase resistance, phase inductance, number of pole pairs and encoder CPR (fixed).

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
    tm1.set_pos_setpoint(8000)

The motor should jump to the commanded positions.