"""
This unit test suite tests functionality of the
Tinymovr Studio using a simulated Tinymovr
device, which is suitable for unit testing.
"""
import random
import time
import can

import tinymovr
from tinymovr import Tinymovr
from tinymovr.constants import ErrorIDs
from tinymovr.iface import IFace
from tinymovr.iface.can_bus import CANBus
from tinymovr.units import get_registry

import unittest

ureg = get_registry()
A = ureg.ampere
ticks = ureg.ticks
s = ureg.second

bustype = "insilico"
channel = "test"


def get_tm() -> Tinymovr:
    can_bus: can.Bus = can.Bus(bustype=bustype, channel=channel)
    iface: IFace = CANBus(can_bus)
    return Tinymovr(node_id=1, iface=iface)


class TestSimulation(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tm: Tinymovr = get_tm()

    def setUp(self):
        self.tm.reset()

    def test_get_device_info(self):
        """
        Test getting of device info
        """
        info = self.tm.device_info
        self.assertGreaterEqual(info.fw_major, 0)
        self.assertGreaterEqual(info.fw_minor, 7)

    def test_version_mismatch(self):
        can_bus: can.Bus = can.Bus(bustype=bustype, channel=channel)
        can_bus.min_studio_version = ["0", "255", "255"] # some impossibly large version
        iface: IFace = CANBus(can_bus)
        with self.assertRaises(AssertionError):
            Tinymovr(node_id=1, iface=iface)

    def test_get_error_idle(self):
        """
        Test successful getting of correct error codes
        in various scenarios
        """
        self.assertFalse(self.tm.state.errors)

    def test_get_error_nocalib(self):
        """
        Test successful getting of correct error codes
        in various scenarios
        """
        self.tm.position_control()
        self.assertIn(ErrorIDs.InvalidState, self.tm.state.errors)

    def test_get_error_calib(self):
        """
        Test successful getting of correct error codes
        in various scenarios
        """
        self.tm.calibrate()  # no need to wait cause it's simulation
        self.tm.position_control()
        self.assertFalse(self.tm.state.errors)

    def test_get_encoder_estimates(self):
        estimates = self.tm.encoder_estimates
        self.assertEqual(estimates.position, 0)
        self.assertEqual(estimates.velocity, 0)

    def test_set_current_control(self):
        self.tm.calibrate()
        self.tm.current_control()
        self.tm.set_cur_setpoint(0.5 * A)
        self.assertEqual(self.tm.Iq.estimate, 0.5 * A)
        time.sleep(0.5)
        self.tm.set_cur_setpoint(-0.5 * A)
        self.assertEqual(self.tm.Iq.estimate, -0.5 * A)
        time.sleep(0.5)
        self.assertLess(abs(self.tm.encoder_estimates.velocity), 500 * ticks / s)

    def test_set_current_control_nounits(self):
        self.tm.calibrate()
        self.tm.current_control()
        self.tm.set_cur_setpoint(0.5)
        self.assertEqual(self.tm.Iq.estimate.magnitude, 0.5)
        time.sleep(0.5)
        self.tm.set_cur_setpoint(-0.5)
        self.assertEqual(self.tm.Iq.estimate.magnitude, -0.5)
        time.sleep(0.5)
        self.assertLess(abs(self.tm.encoder_estimates.velocity.magnitude), 500)

    def test_set_vel_control(self):
        self.tm.calibrate()
        self.tm.current_control()
        self.tm.set_vel_setpoint(1000 * ticks / s)
        time.sleep(0.5)
        self.tm.set_vel_setpoint(-1000 * ticks / s)
        time.sleep(0.5)
        self.assertLess(abs(self.tm.encoder_estimates.position), 500 * ticks)

    def test_set_vel_control_nounits(self):
        self.tm.calibrate()
        self.tm.current_control()
        self.tm.set_vel_setpoint(1000)
        time.sleep(0.5)
        self.tm.set_vel_setpoint(-1000)
        time.sleep(0.5)
        self.assertLess(abs(self.tm.encoder_estimates.position.magnitude), 500)

    def test_get_set_pos_vel(self):
        self.tm.calibrate()
        self.tm.current_control()
        vals = self.tm.get_set_pos_vel_Iq(0, 500 * ticks / s, 0.001 * A)
        self.assertAlmostEqual(vals.position, 0, delta= 1 * ticks)
        self.assertAlmostEqual(vals.velocity_ff, 0, delta= 10 * ticks)
        time.sleep(0.5)
        vals = self.tm.get_set_pos_vel_Iq(0, 0, 0)
        self.assertLess(abs(vals.position.magnitude), 500)


if __name__ == "__main__":
    unittest.main()