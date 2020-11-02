import random
import time
import can

import tinymovr
from tinymovr import Tinymovr, ErrorIDs
from tinymovr.iface import IFace
from tinymovr.iface.can import CAN

import unittest

class TestSetters(unittest.TestCase):

    def setUp(self):
        bustype = "tinymovr_test"
        channel = "test_channel"
        can_bus: can.Bus = can.Bus(bustype=bustype,
                               channel=channel)
        iface: IFace = CAN(can_bus)
        self.tm: Tinymovr = Tinymovr(node_id=1, iface=iface)

    def test_set_current_control(self):
        self.tm.current_control()
        self.tm.set_cur_setpoint(0.5)
        time.sleep(0.5)
        self.tm.set_cur_setpoint(-0.5)
        time.sleep(0.5)
        self.assertLess(abs(self.tm.encoder_estimates.velocity), 500)

    def test_set_vel_control(self):
        self.tm.current_control()
        self.tm.set_vel_setpoint(1000)
        time.sleep(0.5)
        self.tm.set_vel_setpoint(-1000)
        time.sleep(0.5)
        self.assertLess(abs(self.tm.encoder_estimates.position), 500)



if __name__ == '__main__':
    unittest.main()