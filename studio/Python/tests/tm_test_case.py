"""
This unit test suite tests functionality
of Tinymovr boards.
"""

import time
import can
from avlos import ObjectFactory
from tinymovr.isotp_channel import ISOTPChannel

import logging
import unittest


bustype = "slcan"
NODE_ID = 1


class TMTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        '''
        Set up logging, connection and the Tinymovr object
        '''
        cls.logger = logging.getLogger("tinymovr")
        cls.logger.setLevel(logging.DEBUG)
        can_bus = can.Bus(bustype="slcan", channel="auto", node_id=NODE_ID)
        isotp_chan = ISOTPChannel(can_bus, NODE_ID, cls.logger)
        f = ObjectFactory(isotp_chan)
        cls.tm = f.get_object_tree()
        cls.tm.reset()
        time.sleep(3)

    def tearDown(self):
        '''
        Teardown the test
        '''
        self.tm.controller.state = 0

    @classmethod
    def tearDownClass(cls):
        '''
        Teardown the test class
        '''
        cls.tm.reset()
        cls.can_bus.shutdown()

    def try_calibrate(self, *args, **kwargs):
        '''
        Try to calibrate if not calibrated, monitor progress
        and return once calibrated
        '''
        if not self.tm.motor.calibrated:
            self.tm.controller.state = 1 # calibrate
            self.wait_for_calibration(*args, **kwargs)
            self.assertTrue(self.tm.motor.calibrated)

    def wait_for_calibration(self, check_interval=0.05):
        '''
        Continuously query the controller state and return
        once it is in idle state
        '''
        for _ in range(1000):
            if self.tm.controller.state == 0:
                break
            time.sleep(check_interval)
        self.assertEqual(self.tm.controller.state, 0)

    def check_state(self, target_state):
        self.assertEqual(self.tm.controller.state, target_state)