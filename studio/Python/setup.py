"""
Tinymovr Setup Module
Copyright Ioannis Chatzikonstantinou 2020-2023

Standard Python module for setting up the Tinymovr package

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
"""

import pathlib
from setuptools import setup, find_packages

# The directory containing this file
HERE = pathlib.Path(__file__).parent

# The text of the README file
README = (HERE / "README.md").read_text()

setup(
    name="tinymovr",
    # version="", #version will be taken from git tag
    author="Yannis Chatzikonstantinou",
    author_email="info@tinymovr.com",
    description="Tinymovr Studio",
    long_description=README,
    long_description_content_type="text/markdown",
    url="https://github.com/yconst/Tinymovr",
    packages=find_packages(include=["tinymovr", "tinymovr.*"]),
    include_package_data=True,
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.9",
    setuptools_git_versioning={
        "enabled": True,
        "dev_template": "{tag}+post{ccount}",
        "dirty_template": "{tag}+post{ccount}_dirty",
    },
    setup_requires=["setuptools-git-versioning<2"],
    install_requires=[
        "ipython",
        "importlib_resources",
        "python-can",
        "python-can-canine",
        "python-can-slcan_disco",
        "avlos",
        "pyserial",
        "pyusb",
        "pyyaml",
        "docopt",
        "flatten-dict",
        "pint",
    ],
    extras_require={"gui": ["pyside2", "pyqtgraph"]},
    entry_points={
        "console_scripts": [
            "tinymovr_cli=tinymovr.cli:spawn",
            "tinymovr=tinymovr.gui:spawn",
        ]
    },
)
