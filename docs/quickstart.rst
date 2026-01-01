************************
From Unbox to First Spin
************************

Preparation
###########

Welcome to Tinymovr!

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

    pip3 install 'tinymovr[GUI]'
    tinymovr

You should now be looking at the Tinymovr Studio GUI interface.

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

In Tinymovr Studio, navigate to `tm1->controller`. Press the button with the arrow next to the `calibrate` label. Note that after pressing this button, the motor will spin. Ensure the rotor is free of obstructions or loads, and the motor is firmly fixed.

Follow the on-screen prompts. The motor will produce an audible beep and rotate in one direction.
Your Tinymovr is now ready for operation. Navigate to `tm1->motor`. This will reveal identified motor parameters, namely: phase resistance (R), phase inductance (L) and number of pole pairs.

Testing Position Control using the Studio GUI
#############################################

Navigate back to `tm1->controller`. Press the button with the arrow next to the `position_mode` label. Note that after pressing the button, the motor will hold position and may spin. The motor should now be actively holding it's position. Try moving it by hand and you should feel resistance.

To command a new position, navigate to `tm1->controller->position`, and double-click on the value next to the `setpoint` label. This value is expressed in ticks, a unit that denotes 1/8192 of the circle. Type in a new position followed by Enter. The motor should jump to the commanded position.

Connecting More Than 1 Tinymovrs
#################################

Multiple Tinymovr devices can coexist on the same CAN bus network, enabling multi-axis control for applications like robotic arms, gantries, and other coordinated motion systems. The key requirement is that each device must have a unique CAN bus ID. By default, all Tinymovr boards ship with ID 1.

Physical Connections
********************

When connecting multiple Tinymovrs, you'll need to wire them in a daisy-chain topology on the CAN bus:

1. Connect CAN High and CAN Low between devices (CAN High to CAN High, CAN Low to CAN Low)
2. Ensure proper termination resistors are enabled at both ends of the CAN bus. You can use the termination on your CANine adapter (see :ref:`connecting-data`) for one end, and enable the onboard termination resistor on the last Tinymovr in the chain for the other end.
3. Each Tinymovr needs its own power connection

Assigning Unique IDs
********************

To assign different IDs to each board, follow this process:

1. Connect a single Tinymovr to the bus and launch Tinymovr Studio
2. The board will appear as `tm1` with the default ID of 1
3. In the Studio GUI, navigate to the device's CAN settings and change the ID to your desired value (IDs can range from 1 to 1024)
4. Save the configuration using the Save Config button in the GUI
5. Power down the board
6. Repeat the process for each additional Tinymovr, assigning each one a unique ID

Once configured, all devices will automatically appear in Tinymovr Studio with their respective IDs (tm1, tm2, tm3, etc.). You can then control them individually or coordinate their motion for synchronized multi-axis operation.

For detailed step-by-step instructions with screenshots, please see :ref:`multiple-instances`.
