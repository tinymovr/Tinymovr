import random
import time
import can

import tinymovr
from tinymovr import Tinymovr
from iface import CAN
import unittest

class CANReliability(unittest.TestCase):

    def test_can_reliability(self):
        iface = CAN(can.interface.Bus(interface='arduino_can', channel='can0'))
        tm = Tinymovr(node_id=1, iface=iface)
        time.sleep(0.5)
        state = tm.state
        self.assertEqual(state[0], 0, "Error flag present")
        time.sleep(0.1)
        prev_pos = tm.encoder_estimates[0]
        for counter in range(20000):
            pos = tm.encoder_estimates[0]
            # Periodically report iteration
            if counter % 1000 == 0:
                print(counter)
            # Report errors in received position values
            if (abs(prev_pos - pos) > 1000):
                print(str(counter) + ": " + str(pos) + ", " + str(prev_pos))
                pos = prev_pos
            prev_pos = pos
            time.sleep(0.005)

if __name__ == '__main__':
    unittest.main()