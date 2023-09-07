#!/bin/bash

# Check if python3.10 is available
if ! command -v python3.10 &> /dev/null; then
    echo "python3.10 is required but it's not installed."
    exit 1
fi

# Prompt the user for Test PyPI credentials
read -p "Enter your Test PyPI username: " TEST_PYPI_USERNAME
read -sp "Enter your Test PyPI password: " TEST_PYPI_PASSWORD
echo

# Create and activate a virtual environment with Python 3.10
python3.10 -m venv venv
source venv/bin/activate

# Upgrade pip and install dependencies
pip install --upgrade pip
pip install setuptools wheel twine

# Build the package
python setup.py sdist bdist_wheel

# Publish to Test PyPI
twine upload --repository-url https://test.pypi.org/legacy/ dist/* \
  -u $TEST_PYPI_USERNAME -p $TEST_PYPI_PASSWORD

# Install the latest version of tinymovr from Test PyPI without its dependencies
pip install --index-url https://test.pypi.org/simple/ tinymovr --no-deps --upgrade

# Now, install its dependencies
pip install tinymovr

# Set PYTHONPATH to ensure unittest finds the test
export PYTHONPATH=$PWD:$PYTHONPATH

# Run the specific test
python -m unittest tests/test_simulation.py

# Deactivate the virtual environment
deactivate

# Optionally, remove the virtual environment
rm -r venv
