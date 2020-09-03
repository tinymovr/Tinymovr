

Tinymovr Studio Command Reference
#################################


can_config
**********

| **endpoint**: ``0x05``
| **type**: Read-only

Retrieves the CAN configuration.

Fields
------

=============  ===========  =========
Field          Description  Data Type
-------------  -----------  ---------
``id``         CAN Bus ID   uint8
``baud_rate``  Baud Rate    uint16
=============  ===========  =========

Example
-------

.. code-block:: python

    >>>tmx.can_config
    {"id": 1, "baud_rate": 250}


set_can_config()
****************

| **endpoint**: ``0x06``
| **type**: Write-only

Sets the CAN configuration.

Fields
------

=============  ===========  =========
Field          Description  Data Type
-------------  -----------  ---------
``id``         CAN Bus ID   uint8
``baud_rate``  Baud Rate    uint16
=============  ===========  =========

Example
-------

.. code-block:: python

    >>>tmx.set_can_config(id=1, baud_rate=250)