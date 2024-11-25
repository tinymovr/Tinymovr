***************
Troubleshooting
***************

FAQ
***

**Q**: I'm getting "OSError: Could not autodiscover CAN channel" when launching Tinymovr Studio

**A**: This error appear if the CAN adapter is not detected. If using Tinymovr on Windows with a CANtact-compatible CAN Bus adapter, such as CANine, you will need to install an .inf file to enable proper device naming. You can [download the inf file here](https://canable.io/utilities/windows-driver.zip). Extract the archive, right click and select Install.

**Q**: Tinymovr is not detected after launching Tinymovr Studio

**A**: This issue may have several causes, but one of the most common ones is not enabling the termination resistor on the CANine adapter. Please flip the corresponding DIP switch on the CANine and try again, power-cycling the Tinymovr and reconnecting the CANine.

**Q**: I upgraded the firmware and now the board is stuck in DFU mode. Resetting it still gets me to DFU.

**A**: Make sure you have downloaded the **upgrade** variant of the correct binary for your board from the releases page, and flash that.

**Q**: In the Arduino library I'm getting a "Wrong device spec!" error.

**A**: This error appears if the Arduino library is a different version from the Tinymovr firmware. Ensure the Arduino library you downloaded from releases is the same minor version as the firmware you have flashed on your Tinymovr. 

**Q**: How can I use the Tinymovr or Axion R1 actuator with an Arduino? Which CAN library and Arduino hardware to use?

**A**: We've had best results with the Arduino MKR Wifi 1010 and CAN Shield. The Arduino UNO + MCP2515 breakout should also work, but if you are a beginner, try the MKR Wifi 1010.

___

For more answers, please take a look at `Github Discussions <https://github.com/yconst/Tinymovr/discussions>`_ and `Github Issues <https://github.com/yconst/Tinymovr/issues>`_ , and ask any questions there.

You are more than welcome to join the `Tinymovr Discord Server <https://discord.gg/nM7PnXD4>`_ and ask any questions there, or have a chat with us!

