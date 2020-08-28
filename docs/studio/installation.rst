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