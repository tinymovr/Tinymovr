*****************
From Zero to Spin
*****************

Preparation
###########

If you are using a Tinymovr Servo Kit/Tinymovr Dev Kit, please ensure you have completed :ref:`connecting-data` and :ref:`connecting-power`.

If you are using a Tinymovr board in your own setup, please go through :ref:`hardware-overview` and :ref:`hardware-setup`.

If using Tinymovr on Windows with a CANtact-compatible CAN Bus adapter, such as CANine, you will need to install an .inf file to enable proper device naming. You can `download the inf file here <https://canable.io/utilities/windows-driver.zip>`_. Extract the archive, right click and select Install.

If using CAN bus, you may need to enable at least one termination resistor, either on the CANine adapter, or on one Tinymovr. To enable the termination resistor on CANine, you can take a look at the `CANine connectors diagram <https://canine.readthedocs.io/en/latest/canine.html#hardware-configuration>`_.

Before proceeding to the next steps, ensure your Tinymovr is powered up.

Installing and Launching Studio
###############################

Tinymovr can be installed using pip. Python 3.10 or greater is required.

.. note::
   We recommend installing Tinymovr in a virtual environment. `Here is a quick tutorial on how to setup a virtual environment using Conda <https://conda.io/projects/conda/en/latest/user-guide/getting-started.html#managing-environments>`_.


.. code-block:: console

    pip3 install tinymovr
    tinymovr

You should now be looking at the Tinymovr Studio IPython interface.

Installing on Raspberry Pi
##########################

Installation on Raspberry Pi requires a few additional steps.

.. code-block:: console

    sudo apt update
    sudo apt install python3-pip python3-numpy libopenjp2-7 libtiff5
    pip3 install tinymovr

You may also need to append a directory to your PATH variable:

.. code-block:: console

    echo 'export PATH="/home/pi/.local/bin:$PATH"' >> ~/.bashrc
    source ~/.bashrc

Now you should be able to run the Tinymovr CLI:

.. code-block:: console

    tinymovr_cli

Calibrating using the Studio GUI
################################

The Tinymovr Studio GUI offers an overview of the device internals as a tree structure. You can inspect the various values to ensure they are as expected.

The Tinymovr Servo Kit motor and encoder are already calibrated. If you have your own setup, or if you experience problems with prior calibration, you'll need to go through the following brief calibration procedure.

In Tinymovr Studio, navigate to `tmx->controller`. Press the `Calibrate` button. Note that after pressing this button, the motor will spin. Ensure the rotor is free of obstructions or loads, and the motor is firmly fixed.

Follow the on-screen prompts. The motor will produce an audible beep and rotate in one direction.
Your Tinymovr is now ready for operation. Navigate to `tmx->motor`. This will reveal identified motor parameters, namely: phase resistance, phase inductance, number of pole pairs and encoder ticks.

Testing Position Control using the Studio GUI
#############################################

Navigate back to `tmx->controller`. Press the `Position` button. Note that after pressing the button, the motor will hold position and may spin. The motor should now be actively holding it's position. Try moving it by hand and you should feel resistance.

To command a new position, navigate to `tmx->controller->position`, and double-click on the `setpoint` value. Type in a new position followed by Enter. The motor should jump to the commanded position.
