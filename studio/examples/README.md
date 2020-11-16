## Tinymovr Examples

This folder contains example applicationσ of Tinymovr. Currently the following are available:

### Telepresence Demo, Current Mode
File: Telepresence_pos.py

This is a telepresence demo that controls each unit in current mode, and uses the position and velocity error among the two units to generate a current setpoint signal.

### Telepresence Demo, Common Position and Velocity Mode
File: telepresence_torque.py

This is a telepresence demo that always commands the mean of positions and velocities to each controller. This offers better torque tracking at the cost of reduced bandwidth.

Please note that the gains in the above demos are tuned rather conservatively for safety. Start from these values and tune up. Even so, you may experience oscillations depending on your configuration. Please pay attention and always have an e-stop nearby.

