.. _Tuning:

**********
PID Tuning
**********

Tuning PID gains can be an iterative process, and it's essential to approach it systematically to achieve the desired performance. Here's a basic guide for tuning the PID gains for your Tinymovr:

Preliminaries:
##############

- Ensure the motor won't cause damage or injury if it starts to oscillate or behave unexpectedly. Fasten the stator assembly to avoid loosening in case of excessive oscillations.
- If the default gains cause oscillations, reduce all gains until the system is stable.

Tuning the Velocity Controller
##############################

Proportional Gain (P)
---------------------

1. Gradually increase the proportional gain until the system starts to oscillate when given a step change in velocity command.
2. Note this gain value down. This is the critical gain.
3. Set the velocity proportional gain to roughly half of the previous value for a balance of performance and stability.

Integral Gain (I)
-----------------

1. With the proportional gain set as above, slowly increase the integral gain.
2. If the motor starts to oscillate, decrease the integral gain slightly until stability is achieved.
3. The role of the integral gain is to eliminate steady-state errors. Ensure that with the selected gain, errors in the steady state (like due to friction or minor disturbances) are reduced.

Tuning the Position Controller
##############################

Proportional Gain (P):
----------------------

1. Start increasing the proportional gain. The system will use the already-tuned velocity controller as an intermediate step.
2. Increase until you notice overshoot in the position response. If overshooting is excessive or oscillations start, reduce the gain slightly until satisfactory performance is achieved without instability.
3. Ideally, the response should be quick to reach the desired position but without excessive oscillations.

Tips and Considerations:
########################

- Always make changes in small increments.
- If the system becomes unstable at any point, revert to previous settings and proceed more cautiously.
- Remember that real-world systems have nonlinearities and disturbances. Your tuning might need tweaking under different conditions or loads.

Final Testing:
##############

Once you believe you've set the gains appropriately:

1. Test across the entire range of operational conditions to ensure stability and performance.
2. Introduce disturbances if possible (like changes in load) to verify robustness.
3. Adjust as necessary based on real-world performance.
