import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="tinymovr",
    version="0.1.0",
    author="Ioannis Chatzikonstantinou",
    author_email="info@tinymovr.com",
    description="Tinymovr Studio",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/yconst/Tinymovr",
    packages=['tinymovr'],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.6',
    install_requires=[
        "ipython",
        "python-can",
        "pyserial",
        "docopt"
    ],
    entry_points={
        'can.interface': [
            "arduino_can=tinymovr.bus:ArduinoCAN",
        ],
        "console_scripts": [
            "tinymovr=tinymovr.shell:spawn_shell"
        ],
    }
    
)

