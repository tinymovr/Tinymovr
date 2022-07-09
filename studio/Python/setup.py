import pathlib
from setuptools import setup, find_packages

# The directory containing this file
HERE = pathlib.Path(__file__).parent

# The text of the README file
README = (HERE / "README.md").read_text()

setup(
    name="tinymovr",
    version="1.0.0",
    author="Yannis Chatzikonstantinou",
    author_email="info@tinymovr.com",
    description="Tinymovr Studio",
    long_description=README,
    long_description_content_type="text/markdown",
    url="https://github.com/yconst/Tinymovr",
    packages=find_packages(include=["tinymovr", "tinymovr.*"]),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.6",
    install_requires=[
        "ipython",
        "pyside2",
        "pyqtgraph",
        "python-can>=4.0.0.dev0",
        "avlos @ git+https://github.com/tinymovr/avlos.git",
        "pyserial",
        "pyyaml",
        "docopt",
        "flatten-dict",
        "pint",
    ],
    entry_points={
        "console_scripts": [
            "tinymovr_cli=tinymovr.cli:spawn",
            "tinymovr=tinymovr.gui:spawn",
        ],
        "can.interface": [
            "insilico=tinymovr.bus:InSilico",
        ],
    },
)
