![C/C++ CI](https://github.com/yconst/Tinymovr-Firmware/workflows/C/C++%20CI/badge.svg)

Firmware for the Tinymovr R3 platform.

Tinymovr uses the PAC5527 controller, MA702 14-bit magnetic encoder, FDMD8530 FETs and TJA1044 CAN bus transceiver.

__NOTE: This firmware no longer supports earlier versions of Tinymovr based on the PAC5523 (R1 and R2).__

### Hardware

You will need a Segger J-Link adapter to program the Tinymovr board. Instructions on how to connect the adapter can be found in the Tinymovr documentation.

Unfortunately original J-Link adapters are quite expensive. A more affordable option is the J-Link EDU adapter at around $60 or the J-Link EDU mini adapter at around $20. In addition, there are J-Link clones that can be purchased for very low prices on ebay or Aliexpress. However, reliability of these clones is not guaranteed.

### Supported Environments

This project can be imported in Eclipse. The Qorvo-supplied Eclipse distribution is recommended. Full debug capabilities are available. This is the officially supported development environment.

This project can be also opened in VSCode. Full debugging capabilities are available. This is an unofficial development setup that is not supported by Qorvo. You will need the gcc-arm-none-eabi toolchain, the J-Link toolchain and also you will need to install the C/C++ Intellisense and Cortex-Debug extensions.
