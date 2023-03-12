![FW Check Build](https://github.com/yconst/tinymovr/workflows/Tinymovr%20Firmware%20Check%2FBuild/badge.svg)

> **Warning**
>
> This is the legacy 0.x.x version of the Tinymovr Firmware and Studio app. For the the latest stable version, check out the [main branch](https://github.com/tinymovr/Tinymovr/).

Firmware for the Tinymovr R3 platform (PAC5527).

### Supported Environments

This project can be imported in Eclipse. The Qorvo-supplied Eclipse distribution is recommended. Full debug capabilities are available. This is the officially supported development environment.

It is possible to build the project using make and gcc, however this configuration is outdated and should be avoided. In particular, this generated a non-bootloader enabled binary and may brick your board.


### Programming

You can program Tinymovr using the bootloader through UART or using a Segger J-Link probe. Different Eclipse configurations are needed for each option. Please refer to ActiveFlashLight documentation included with Qorvo Bootloaded package for more information.


### Debugging

You will need a Segger J-Link adapter to debug the Tinymovr board. Instructions on how to connect the adapter can be found in the Tinymovr documentation.

Unfortunately original J-Link adapters are quite expensive. A more affordable option is the J-Link EDU adapter at around $60 or the J-Link EDU mini adapter at around $20. In addition, there are J-Link clones that can be purchased for very low prices on ebay or Aliexpress. However, reliability of these clones is not guaranteed.
