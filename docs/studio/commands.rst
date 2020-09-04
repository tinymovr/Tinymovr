

.. _command-reference:

Tinymovr Studio Command Reference
#################################

state
*****

| **endpoint**: ``0x03``
| **type**: Read-only

Retrieves the CAN configuration.

Fields
------

=============  =============  =========  ===========
Field          Description    Data Type  Data Offset
-------------  -------------  ---------  -----------
``error``      Error Code     uint8      0
``state``      Control State  uint8      1
``mode``       Control Mode   uint8      2
=============  =============  =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.state
    {"error": 0, "state": 0, "mode": 0}


set_state()
***********

| **endpoint**: ``0x07``
| **type**: Write-only

Sets the CAN configuration.

Fields
------

=============  =============  =========  ===========
Field          Description    Data Type  Data Offset
-------------  -------------  ---------  -----------
``state``      Control State  uint8      1
``mode``       Control Mode   uint8      2
=============  =============  =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.set_state(state=0, mode=0)


can_config
**********

| **endpoint**: ``0x05``
| **type**: Read-only

Retrieves the CAN configuration.

Fields
------

=============  ===========  =========  ===========
Field          Description  Data Type  Data Offset
-------------  -----------  ---------  -----------
``id``         CAN Bus ID   uint8      0
``baud_rate``  Baud Rate    uint16     1
=============  ===========  =========  ===========

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

=============  ===========  =========  ===========
Field          Description  Data Type  Data Offset
-------------  -----------  ---------  -----------
``id``         CAN Bus ID   uint8      0
``baud_rate``  Baud Rate    uint16     1
=============  ===========  =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.set_can_config(id=1, baud_rate=250)


encoder_estimates
*****************

| **endpoint**: ``0x09``
| **type**: Read-only

Retrieves the position and velocity encoder estimates.

Fields
------

=============  =================   =========  ===========
Field          Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``position``   Position Estimate   float32    0
``velocity``   Velocity Estimate   float32    4
=============  =================   =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.encoder_estimates
    {"position": 1000.0, "velocity": 0.0}


setpoints
*********

| **endpoint**: ``0x09``
| **type**: Read-only

Retrieves the position and velocity setpoints of the controller.

Fields
------

=============  =================   =========  ===========
Field          Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``position``   Position Setpoint   float32    0
``velocity``   Velocity Setpoint   float32    4
=============  =================   =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.setpoints
    {"position": 1000.0, "velocity": 0.0}


set_pos_setpoint()
******************

| **endpoint**: ``0x0C``
| **type**: Write-only

Sets the position setpoint, and optionally velocity and current feed-forward values.

Fields
------

=============  =================   =========  ===========
Field          Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``position``   Position Setpoint   float32    0
``velocity``   Velocity Setpoint   int16      4
``current``    Current Setpoint    int16      6
=============  =================   =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.set_pos_setpoint(1000.0)

.. code-block:: python

    >>>tmx.set_pos_setpoint(position=1000.0, velocity=10000.0, current=0.0)


set_vel_setpoint()
******************

| **endpoint**: ``0x0D``
| **type**: Write-only

Sets the velocity setpoint, and optionally current feed-forward value.

Fields
------

=============  =================   =========  ===========
Field          Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``velocity``   Velocity Setpoint   float32    0
``current``    Current Setpoint    float32    4
=============  =================   =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.set_vel_setpoint(10000.0)

.. code-block:: python

    >>>tmx.set_vel_setpoint(velocity=10000.0, current=0.0)


set_cur_setpoint()
******************

| **endpoint**: ``0x0E``
| **type**: Write-only

Sets the current (Iq) setpoint.

Fields
------

=============  =================   =========  ===========
Field          Description         Data Type  Data Offset
-------------  -----------------   ---------  -----------
``current``    Current Setpoint    float32    0
=============  =================   =========  ===========

Example
-------

.. code-block:: python

    >>>tmx.set_cur_setpoint(0.5)