
> **Warning**
>
> You are browsing the legacy 0.x.x version of the Tinymovr Firmware and Studio app. For the the latest stable version, check out the [main branch](https://github.com/tinymovr/Tinymovr/).

## Tinymovr Studio Python

This is the Python client library and application that allows easy interfacing with the Tinymovr motor control board.

### Installing using pip

This is the most straightforward method to install Tinymovr studio and have access to hardware. We use pip3 to ensure Tinymovr is installed using Python 3 (required).

    pip3 install tinymovr

#### Installing earlier versions

The above command will install the latest version of Tinymovr Studio available on PyPI. In order to maintain backwards compatibility with existing projects, earlier versions of Tinymovr are made available. Currently, the following versions are recommended:

- [Tinymovr Studio Latest](https://pypi.org/project/tinymovr/) (latest version)
- [Tinymovr Studio 2.7](https://pypi.org/project/tinymovr/0.2.7/) (latest version without units)

### Installing using git clone

First clone the Tinymovr repo to a local directory:

    git clone https://github.com/yconst/Tinymovr

Then cd to the cloned repo directory and install in developer mode:

    cd Tinymovr/studio/Python
    pip3 install -e .

### Launching the command line app

    tinymovr

### More information

For documentation:

https://tinymovr.readthedocs.io

For more information on Tinymovr:

https://hackaday.io/project/168650-tinymovr-motor-controller
