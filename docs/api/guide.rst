*********
API Guide
*********


Overview
########


Installation
############

Tinymovr API is part of Tinymovr Studio. For help installing Studio, please take a look at the installation guide.

API Command Reference
#####################


CAN Config (``0x05``) *R*
=========================

Retrieves the CAN configuration.

Message Format ``011-----``
---------------------------

===========  =========  ============
Description  Data Type  Offset Bytes
-----------  ---------  ------------
CAN Bus ID   uint8      0
Baud Rate    uint16     1
===========  =========  ============

