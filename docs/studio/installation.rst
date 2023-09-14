.. _studio-installation:

*******************
Studio Installation
*******************

Tinymovr Studio is a cross-platform GUI application, CLI application, and Python library that offers easy access to all of Tinymovr's functionality. 

Studio requires Python 3.9 or newer.

Preparation
###########

If using Windows with a CANtact-compatible CAN Bus adapter, such as CANine, you will need to install an .inf file to enable proper device naming. You can `download the inf file here <https://canable.io/utilities/windows-driver.zip>`_. Extract the archive, right click and select Install.

Using pip
#########

This is the most straightforward method to install Tinymovr studio and have access to hardware. The following command will install Tinymovr with the dependencies required for the Qt-based Tinymovr Studio GUI:

.. code-block:: console

    pip3 install 'tinymovr[GUI]'

If you don't plan to use the GUI, you can skip installing some dependencies using the following installation command instead:

.. code-block:: console

    pip3 install tinymovr

.. code-block:: console

    tinymovr

You should now be looking at the Tinymovr GUI. Alternatively, to start the IPython-based Command Line Interface (CLI):

.. code-block:: console

    tinymovr_cli

Anaconda Installation
---------------------

Tinymovr can be installed inside a Virtualenv or Anaconda environment. 

With Anaconda, create a new environment:

.. code-block:: console

    conda create --name tinymovr python=3.10 -y
    conda activate tinymovr

Alternatively you can use any Python version >= 3.9.

Then simply install and run Tinymovr:

.. code-block:: console

    pip install 'tinymovr[GUI]'
    tinymovr

Using git clone
###############

.. note::
   The master branch of the Github repository represents the state of art of development, and it may contain bugs.
   For a stable version, especially if you are starting with the project, please consider installing Tinymovr Studio using pip as shown above.

First clone the Tinymovr repo to a local directory:

.. code-block:: console

    git clone https://github.com/yconst/Tinymovr

Then cd to the cloned repo directory and install in developer mode:

.. code-block:: console

    cd Tinymovr/studio/Python
    pip3 install -e .