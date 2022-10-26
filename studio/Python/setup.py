import pathlib
from setuptools import setup, find_packages

# The directory containing this file
HERE = pathlib.Path(__file__).parent

# The text of the README file
README = (HERE / "README.md").read_text()

setup(
    name="tinymovr",
    #version="", #version will be taken from git tag
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
    python_requires=">=3.9",
    setuptools_git_versioning={
        "enabled": True,
        "dev_template": "{tag}.post{ccount}",
        "dirty_template": "{tag}.post{ccount}.dirty"
    },
    setup_requires=["setuptools-git-versioning<2"],
    install_requires=[
        "ipython",
        "python-can>=4.0.0.dev0",
        "avlos @ git+https://github.com/tinymovr/avlos.git",
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
        ],
        # "can.interface": [
        #     "insilico=tinymovr.bus:InSilico",
        # ],
    },
)
