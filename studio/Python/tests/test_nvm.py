"""
Tinymovr Board Config Tests
Copyright Ioannis Chatzikonstantinou 2020-2023

Tests saving and loading Tinymovr configuration to/from NVRAM.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
"""

import random
import time

from avlos.unit_field import get_registry

import unittest
from tests import TMTestCase

ureg = get_registry()
A = ureg.ampere
tick = ureg.ticks
s = ureg.second
tsleep = 0.30


class TestNVM(TMTestCase):
    def test_a_state_errors(self):
        """
        Test state transitions
        WARNING: This may perform one NVRAM erase cycle.
        """
        if self.tm.motor.calibrated:
            self.erase_config()
            time.sleep(0.2)

        self.reset_and_wait()
        self.check_state(0)
        self.assertEqual(self.tm.motor.calibrated, False)
        self.tm.controller.position_mode()
        self.check_state(0)
        self.reset_and_wait()
        self.check_state(0)
        self.assertEqual(self.tm.motor.calibrated, False)
        self.try_calibrate()
        self.assertEqual(self.tm.motor.calibrated, True)
        self.check_state(0)
        time.sleep(0.2)
        self.tm.controller.position_mode()
        self.check_state(2)
        time.sleep(0.2)
        self.tm.controller.calibrate()
        self.check_state(0)

        self.reset_and_wait()

    def test_b_save_load_config(self):
        """
        Test erasing, saving and loading of config.
        WARNING: This will perform one NVRAM write and two erase cycles.
        """
        self.check_state(0)
        self.erase_config()
        time.sleep(0.2)
        self.assertEqual(self.tm.motor.calibrated, False)
        self.try_calibrate()
        self.assertEqual(self.tm.motor.calibrated, True)
        R = self.tm.motor.R
        L = self.tm.motor.L
        pole_pairs = self.tm.motor.pole_pairs
        self.save_config()
        time.sleep(0.2)
        self.reset_and_wait()
        self.assertEqual(self.tm.motor.calibrated, True)
        self.assertAlmostEqual(R, self.tm.motor.R)
        self.assertAlmostEqual(L, self.tm.motor.L)
        self.assertAlmostEqual(pole_pairs, self.tm.motor.pole_pairs)
        self.erase_config()
        time.sleep(0.2)

    def test_c_parameter_persistence(self):
        """
        Test persisting parameters across config saves.
        WARNING: This will perform one NVRAM write and two erase cycles.
        """
        self.check_state(0)
        self.erase_config()
        time.sleep(0.2)

        self.tm.controller.position.p_gain = 30
        self.tm.controller.velocity.p_gain = 3e-5
        self.tm.controller.velocity.i_gain = 2e-2
        self.tm.controller.velocity.deadband = 100
        self.tm.controller.velocity.limit = 120000
        self.tm.controller.current.Iq_limit = 18
        self.save_config()
        
        time.sleep(0.2)
        self.reset_and_wait()
        self.assertAlmostEqual(self.tm.controller.position.p_gain, 30) #  * 1 / s
        self.assertAlmostEqual(self.tm.controller.velocity.p_gain, 3e-5) #  * A * s / tick
        self.assertAlmostEqual(self.tm.controller.velocity.i_gain, 2e-2) #  * A * s / tick
        self.assertAlmostEqual(self.tm.controller.velocity.deadband, 100 * tick) 
        self.assertAlmostEqual(self.tm.controller.velocity.limit, 120000 * tick / s)
        self.assertAlmostEqual(self.tm.controller.current.Iq_limit, 18 * A)
        self.erase_config()
        time.sleep(0.2)

    def test_d_position_control_w_loaded_config(self):
        """
        Test position control after saving and loading config.
        WARNING: This will perform one NVRAM write and two erase cycles.
        """
        hw_rev = self.tm.hw_revision

        self.check_state(0)
        self.erase_config()
        time.sleep(0.2)

        self.try_calibrate()

        # Define hardware-specific parameters
        if hw_rev > 20:
            self.tm.controller.velocity.limit = 85000
            self.tm.controller.velocity.deadband = 500
            settle_time = 0.50
            tick_range = 8000
            tolerance = 5000
            pos_p_gain = 5
            vel_p_gain = 2e-5
            vel_i_gain = 0
        else:
            self.tm.controller.velocity.limit = 120000
            self.tm.controller.velocity.deadband = 1000
            settle_time = 0.50
            tick_range = 18000
            tolerance = 3000
            pos_p_gain = None
            vel_p_gain = None
            vel_i_gain = None

        # Configure gains before entering position mode
        self.tm.controller.current.Iq_limit = 5
        if pos_p_gain is not None:
            self.tm.controller.position.p_gain = pos_p_gain
        if vel_p_gain is not None:
            self.tm.controller.velocity.p_gain = vel_p_gain
        if vel_i_gain is not None:
            self.tm.controller.velocity.i_gain = vel_i_gain

        self.tm.controller.position_mode()
        self.check_state(2)

        for _ in range(15):
            new_pos = random.uniform(-tick_range, tick_range)
            self.tm.controller.position.setpoint = new_pos * tick
            time.sleep(settle_time)
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.position_estimate,
                self.tm.controller.position.setpoint,
                delta=tolerance * tick,
            )

        self.tm.controller.idle()

        time.sleep(0.1)

        self.save_config()

        time.sleep(0.2)

        self.reset_and_wait()

        self.tm.controller.position_mode()
        self.check_state(2)

        for _ in range(15):
            new_pos = random.uniform(-tick_range, tick_range)
            self.tm.controller.position.setpoint = new_pos * tick
            time.sleep(tsleep)
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.position_estimate,
                self.tm.controller.position.setpoint,
                delta=tolerance * tick,
            )

        self.erase_config()
        time.sleep(0.2)

    def test_e_save_settings_and_reset(self):
        """
        Test case for issue reported by @Phosfor on Discord:
        1. Configure current limits etc.
        2. Calibrate the controller
        3. Make sure everything is working (i.e. enter current/velocity mode)
        4. Save the configuration
        5. Reset the tinymovr; all settings are still correct and the tinymovr reports that it is calibrated
        6. Try to enter current/velocity mode again; the tinymovr says it is in the correct mode and in the control state, but the motor is not turning
        """
        hw_rev = self.tm.hw_revision

        iq_limit = 5
        
        # Define hardware-specific parameters
        if hw_rev > 20:
            velocity_limit = 85000
            vel_i_gain = 0
            vel_p_gain = 3e-5
            vel_deadband = 50
            max_vel = 85000
            max_accel = 500000
            max_decel = 500000
            velocity_range = 30000
            settle_time = 0.50
        else:
            velocity_limit = 120000
            vel_i_gain = 1e-5
            vel_p_gain = 6e-5
            vel_deadband = 50
            max_vel = 120000
            max_accel = 500000
            max_decel = 500000
            velocity_range = 50000
            settle_time = 0.50

        max_delta = 5000

        # Step 1: Configure current limits etc.
        self.check_state(0)
        self.tm.erase_config()
        time.sleep(0.2)

        self.tm.controller.current.Iq_limit = iq_limit
        self.tm.controller.velocity.limit = velocity_limit * tick / s
        self.tm.controller.velocity.i_gain = vel_i_gain
        self.tm.controller.velocity.p_gain = vel_p_gain
        self.tm.controller.velocity.deadband = vel_deadband * tick
        self.tm.traj_planner.max_vel = max_vel * tick / s
        self.tm.traj_planner.max_accel = max_accel * tick / s
        self.tm.traj_planner.max_decel = max_decel * tick / (s * s)

        # Step 2: Calibrate the controller
        self.try_calibrate()
        self.assertEqual(self.tm.motor.calibrated, True)

        # Step 3: Make sure everything is working (i.e. enter current/velocity mode)
        self.tm.controller.velocity_mode()
        self.check_state(2)

        for _ in range(5):
            new_velocity = random.uniform(-velocity_range, velocity_range)
            self.tm.controller.velocity.setpoint = new_velocity * tick / s
            time.sleep(settle_time)
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.velocity_estimate,
                self.tm.controller.velocity.setpoint,
                delta=max_delta * tick / s,
            )

        self.tm.controller.idle()
        time.sleep(0.1)

        # Step 4: Save the configuration
        self.tm.save_config()
        time.sleep(0.2)

        # Step 5: Reset the tinymovr; all settings are still correct and the tinymovr reports that it is calibrated
        self.reset_and_wait()
        self.assertEqual(self.tm.motor.calibrated, True)
        self.assertAlmostEqual(self.tm.controller.current.Iq_limit, iq_limit * A)
        self.assertAlmostEqual(self.tm.controller.velocity.limit, velocity_limit * tick / s)
        self.assertAlmostEqual(self.tm.controller.velocity.i_gain, vel_i_gain)
        self.assertAlmostEqual(self.tm.controller.velocity.p_gain, vel_p_gain)
        self.assertAlmostEqual(self.tm.controller.velocity.deadband, vel_deadband * tick)
        self.assertAlmostEqual(self.tm.traj_planner.max_vel, max_vel * tick / s)
        self.assertAlmostEqual(self.tm.traj_planner.max_accel, max_accel * tick / s)
        self.assertAlmostEqual(self.tm.traj_planner.max_decel, max_decel * tick / (s * s))

        # Step 6: Try to enter current/velocity mode again; the tinymovr says it is in the correct mode and in the control state, but the motor is not turning (and I don't hear any switching noise that I usually hear when the tinymovr is in the active state)
        self.tm.controller.velocity_mode()
        self.check_state(2)

        for _ in range(5):
            new_velocity = random.uniform(-velocity_range, velocity_range)
            self.tm.controller.velocity.setpoint = new_velocity * tick / s
            time.sleep(tsleep)
            # Here we expect the motor not to turn, so we check for lack of movement
            self.assertAlmostEqual(
                self.tm.sensors.user_frame.velocity_estimate,
                self.tm.controller.velocity.setpoint,
                delta=max_delta * tick / s,
            )

        self.tm.controller.idle()
        time.sleep(0.1)

        self.tm.erase_config()
        time.sleep(0.2)

    def test_f_wear_leveling_rotation(self):
        """
        Test that configs rotate through all available wear leveling slots.
        WARNING: This will perform (num_slots + 2) NVRAM write/erase cycles.
        """
        self.check_state(0)

        # Start fresh
        self.erase_config()
        time.sleep(0.2)

        # Get number of slots (adapts automatically to config size)
        num_slots = self.get_nvm_num_slots()

        # Skip if wear leveling debug endpoints not available
        if num_slots is None:
            self.skipTest("Wear leveling debug endpoints not available in this protocol version")

        # Verify we have at least 2 slots for wear leveling
        self.assertGreaterEqual(num_slots, 2, "Need at least 2 slots for wear leveling")

        # Test rotation through all slots + 1 (to verify wraparound)
        for i in range(num_slots + 1):
            # Modify a parameter to ensure config changes
            self.tm.controller.velocity.p_gain = 1e-5 + i * 1e-6

            # Save config
            self.save_config()
            time.sleep(0.2)

            # Check that we're in the expected slot
            current_slot = self.get_nvm_current_slot()
            expected_slot = i % num_slots  # After i+1 saves, we should be in slot i

            self.assertEqual(current_slot, expected_slot,
                f"Slot rotation failed at write {i+1}: expected slot {expected_slot}, got {current_slot}")

        # Verify write count increased
        write_count = self.get_nvm_write_count()
        self.assertGreaterEqual(write_count, num_slots + 1,
            f"Write count should be at least {num_slots + 1}, got {write_count}")

        # Reset and verify most recent config loaded
        self.reset_and_wait()

        # The last value we wrote should be loaded
        last_value = 1e-5 + num_slots * 1e-6
        self.assertAlmostEqual(self.tm.controller.velocity.p_gain, last_value,
            places=8, msg="Most recent config not loaded after reset")

        # Clean up
        self.erase_config()
        time.sleep(0.2)

    def test_g_stress_extended_write_cycles(self):
        """
        Stress test: 100+ consecutive writes to verify wear leveling reliability.
        WARNING: This will perform 100 NVRAM write cycles.
        """
        self.check_state(0)

        # Start fresh
        self.erase_config()
        time.sleep(0.2)

        # Get number of slots
        num_slots = self.get_nvm_num_slots()

        # Skip if wear leveling debug endpoints not available
        if num_slots is None:
            self.skipTest("Wear leveling debug endpoints not available in this protocol version")

        # Calibrate first
        self.try_calibrate()

        # Perform 100 consecutive writes
        num_writes = 100

        for i in range(num_writes):
            # Modify a parameter to ensure config changes
            self.tm.controller.velocity.p_gain = 1e-5 + i * 1e-7

            # Save config
            self.save_config()
            time.sleep(0.05)

            # Verify write count increases monotonically
            write_count = self.get_nvm_write_count()
            self.assertEqual(write_count, i + 1,
                f"Write count mismatch at write {i+1}: expected {i+1}, got {write_count}")

        # Verify final write count
        final_write_count = self.get_nvm_write_count()
        self.assertEqual(final_write_count, num_writes,
            f"Final write count should be {num_writes}, got {final_write_count}")

        # Verify parameter value is correct
        expected_p_gain = 1e-5 + (num_writes - 1) * 1e-7
        self.assertAlmostEqual(self.tm.controller.velocity.p_gain, expected_p_gain,
            places=10, msg="Parameter value corrupted after extended writes")

        # Reset and verify most recent config loaded
        self.reset_and_wait()

        self.assertAlmostEqual(self.tm.controller.velocity.p_gain, expected_p_gain,
            places=10, msg="Most recent config not loaded after reset")

        # Clean up
        self.erase_config()
        time.sleep(0.2)

    def test_i_slot_fill_and_multiple_wraparounds(self):
        """
        Test that configs correctly wrap around when filling all slots multiple times.
        WARNING: This will perform (num_slots * 3) NVRAM write cycles.
        """
        self.check_state(0)

        # Start fresh
        self.erase_config()
        time.sleep(0.2)

        # Get number of slots
        num_slots = self.get_nvm_num_slots()

        # Skip if wear leveling debug endpoints not available
        if num_slots is None:
            self.skipTest("Wear leveling debug endpoints not available in this protocol version")

        # Verify we have at least 2 slots for wear leveling
        self.assertGreaterEqual(num_slots, 2, "Need at least 2 slots for wear leveling")

        # Calibrate first
        self.try_calibrate()

        # Write 3× num_slots to force multiple wraparounds
        total_writes = num_slots * 3

        for i in range(total_writes):
            # Modify a parameter to ensure config changes
            self.tm.controller.position.p_gain = 10 + i * 0.1

            # Save config
            self.save_config()
            time.sleep(0.05)

            # Check that we're in the expected slot
            current_slot = self.get_nvm_current_slot()
            expected_slot = i % num_slots

            self.assertEqual(current_slot, expected_slot,
                f"Slot rotation failed at write {i+1}: expected slot {expected_slot}, got {current_slot}")

        # Verify final slot is correct (should be at (total_writes - 1) % num_slots)
        final_slot = self.get_nvm_current_slot()
        expected_final_slot = (total_writes - 1) % num_slots
        self.assertEqual(final_slot, expected_final_slot,
            f"Final slot should be {expected_final_slot}, got {final_slot}")

        # Reset and verify most recent config loaded
        self.reset_and_wait()

        # The last value we wrote should be loaded
        last_value = 10 + (total_writes - 1) * 0.1
        self.assertAlmostEqual(self.tm.controller.position.p_gain, last_value,
            places=1, msg="Most recent config not loaded after multiple wraparounds")

        # Clean up
        self.erase_config()
        time.sleep(0.2)

    def test_j_write_count_never_decreases(self):
        """
        Test that write count is strictly monotonic increasing.
        """
        self.check_state(0)

        # Start fresh
        self.erase_config()
        time.sleep(0.2)

        # Verify initial write count is 0
        initial_write_count = self.get_nvm_write_count()

        # Skip if wear leveling debug endpoints not available
        if initial_write_count is None:
            self.skipTest("Wear leveling debug endpoints not available in this protocol version")

        self.assertEqual(initial_write_count, 0, "Fresh erase should have write count = 0")

        # Calibrate first
        self.try_calibrate()

        # Perform 20 writes
        prev_count = 0
        for i in range(20):
            # Modify a parameter to ensure config changes
            self.tm.controller.velocity.deadband = i * 10 * tick

            # Save config
            self.save_config()
            time.sleep(0.05)

            # Check that write count increased
            current_count = self.get_nvm_write_count()
            self.assertGreater(current_count, prev_count,
                f"Write count must strictly increase: prev={prev_count}, current={current_count}")
            self.assertEqual(current_count, i + 1,
                f"Write count should increment by 1 per save: expected {i+1}, got {current_count}")
            prev_count = current_count

        # Reset and verify write count doesn't change
        self.reset_and_wait()
        post_reset_count = self.get_nvm_write_count()
        self.assertEqual(post_reset_count, prev_count,
            f"Reset should not change write count: expected {prev_count}, got {post_reset_count}")

        # Clean up
        self.erase_config()
        time.sleep(0.2)

    def test_l_post_erase_state_clean(self):
        """
        Test that erase properly resets all wear leveling state.
        WARNING: This will perform 5 NVRAM write and 1 erase cycle.
        """
        self.check_state(0)

        # Get number of slots
        num_slots = self.get_nvm_num_slots()

        # Skip if wear leveling debug endpoints not available
        if num_slots is None:
            self.skipTest("Wear leveling debug endpoints not available in this protocol version")

        # Calibrate and perform some writes
        self.try_calibrate()

        for i in range(5):
            self.tm.controller.position.p_gain = 20 + i
            self.save_config()
            time.sleep(0.05)

        # Verify non-zero state before erase
        write_count_before = self.get_nvm_write_count()
        current_slot_before = self.get_nvm_current_slot()

        self.assertGreater(write_count_before, 0, "Write count should be > 0 before erase")
        self.assertGreaterEqual(current_slot_before, 0, "Current slot should be >= 0 before erase")

        # Erase and verify clean state
        self.erase_config()
        time.sleep(0.2)

        # Check all wear leveling state is reset
        write_count_after = self.get_nvm_write_count()
        current_slot_after = self.get_nvm_current_slot()

        self.assertEqual(write_count_after, 0, "Write count should be 0 after erase")
        self.assertEqual(current_slot_after, 0, "Current slot should be 0 after erase")

        # Verify motor becomes uncalibrated
        self.assertEqual(self.tm.motor.calibrated, False, "Motor should be uncalibrated after erase")

        # Verify system can be recalibrated and saved successfully
        self.try_calibrate()
        self.tm.controller.velocity.p_gain = 5e-5
        self.save_config()
        time.sleep(0.2)

        # Verify write count is 1 after first save post-erase
        write_count_after_save = self.get_nvm_write_count()
        self.assertEqual(write_count_after_save, 1, "Write count should be 1 after first save post-erase")

        # Clean up
        self.erase_config()
        time.sleep(0.2)

    def test_m_can_id_change_forces_slot_0_and_dfu(self):
        """
        Test that changing CAN ID forces write to slot 0, and verify ID persists in DFU mode.
        This ensures slot 0 always contains the current CAN ID.
        WARNING: This will perform multiple NVRAM write cycles and enter DFU mode.
        """
        self.check_state(0)

        # Start fresh
        self.erase_config()
        time.sleep(0.2)

        # Get number of slots
        num_slots = self.get_nvm_num_slots()

        # Skip if wear leveling debug endpoints not available
        if num_slots is None:
            self.skipTest("Wear leveling debug endpoints not available in this protocol version")

        # Calibrate first
        self.try_calibrate()

        # Save initial config with node ID 1 (default)
        initial_node_id = self.tm.comms.can.id
        self.save_config()
        time.sleep(0.2)

        # Should be in slot 0 after first save
        self.assertEqual(self.get_nvm_current_slot(), 0,
            "First save should be in slot 0")

        # Perform multiple saves to move to different slots
        for i in range(3):
            self.tm.controller.velocity.p_gain = 1e-5 + i * 1e-6
            self.save_config()
            time.sleep(0.1)

        # Verify we moved past slot 0
        current_slot_before_id_change = self.get_nvm_current_slot()
        self.assertNotEqual(current_slot_before_id_change, 0,
            "Should have moved past slot 0 after multiple saves")

        # Change CAN ID
        new_node_id = 5
        self.tm.comms.can.id = new_node_id
        time.sleep(0.2)

        # Recreate device object with new node ID
        from tinymovr.config import create_device
        tm_new = create_device(node_id=new_node_id)

        # Verify ID changed
        self.assertEqual(tm_new.comms.can.id, new_node_id,
            f"CAN ID should be {new_node_id}")

        # Save config - should force write to slot 0
        tm_new.save_config()
        time.sleep(0.2)

        # Verify we're back in slot 0
        self.assertEqual(tm_new.nvm.current_slot, 0,
            "Changing CAN ID should force write to slot 0")

        # Reset and verify new ID persists
        tm_new.reset()
        time.sleep(0.5)

        # Recreate device object after reset
        tm_new = create_device(node_id=new_node_id)

        self.assertEqual(tm_new.comms.can.id, new_node_id,
            f"CAN ID should persist as {new_node_id} after reset")
        self.assertEqual(tm_new.motor.calibrated, True,
            "Motor should remain calibrated after reset")

        # Verify still in slot 0
        self.assertEqual(tm_new.nvm.current_slot, 0,
            "After reset, should still be using slot 0")

        # Enter DFU mode to verify CAN ID persists there
        tm_new.enter_dfu()
        time.sleep(0.5)

        # Create bootloader device instance
        bl = create_device(node_id=new_node_id)

        # Verify CAN ID matches in DFU mode
        self.assertEqual(bl.node_id, new_node_id,
            f"CAN ID in DFU mode should be {new_node_id}")

        # Exit DFU and return to normal operation
        bl.reset()
        time.sleep(0.5)

        # Recreate device object after exiting DFU
        tm_new = create_device(node_id=new_node_id)

        # Verify CAN ID still correct after exiting DFU
        self.assertEqual(tm_new.comms.can.id, new_node_id,
            f"CAN ID should still be {new_node_id} after exiting DFU")

        # Change ID back and verify slot 0 write again
        tm_new.comms.can.id = initial_node_id
        time.sleep(0.2)

        # Recreate device object with original node ID
        self.tm = create_device(node_id=initial_node_id)

        self.save_config()
        time.sleep(0.2)

        # Verify back in slot 0
        self.assertEqual(self.get_nvm_current_slot(), 0,
            "Changing CAN ID back should force write to slot 0 again")

        # Reset and verify original ID restored
        self.reset_and_wait()
        self.assertEqual(self.tm.comms.can.id, initial_node_id,
            f"CAN ID should be back to {initial_node_id}")

        # Clean up
        self.erase_config()
        time.sleep(0.2)


if __name__ == "__main__":
    unittest.main()
