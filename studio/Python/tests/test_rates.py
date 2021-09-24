'''
This unit test suite tests the rate of CAN messaging
on the bus. Measurement is performed while Tinymovr
is in position control mode.
'''
import time

import tinymovr
from tinymovr import Tinymovr
from tinymovr.constants import ErrorIDs

import unittest
from tests import TMTestCase


iterations = 5000


class TestRates(TMTestCase):

    def test_round_trip_time(self):
        '''
        Test round-trip message time (2 packets)
        '''
        self.try_calibrate()
        self.tm.position_control()
        time.sleep(0.2)
        elapsed_time()
        sum = 0
        for _ in range(iterations):
            sum += self.tm.encoder_estimates.position
        res = elapsed_time()
        print("Round-trip time (2 packets): " + str(res/iterations) + " seconds")

    def test_round_trip_time_with_write(self):
        '''
        Test round-trip message time of r/w endpoints (2 packets)
        '''
        self.try_calibrate()
        self.tm.position_control()
        time.sleep(0.2)
        elapsed_time()
        sum = 0
        pos = self.tm.encoder_estimates.position
        for _ in range(iterations):
            sum += self.tm.get_set_pos_vel(pos, 0).position
        res = elapsed_time()
        print("Round-trip time (2 packets, rw): " + str(res/iterations) + " seconds")


def elapsed_time(prefix=''):
    e_time = time.time()
    if not hasattr(elapsed_time, 's_time'):
        elapsed_time.s_time = e_time
    else:
        res = e_time - elapsed_time.s_time
        elapsed_time.s_time = e_time
        return res