import random
import time

import tinymovr
from tinymovr import Tinymovr
import unittest

class RandomWalk(unittest.TestCase):

    def test_random_walk(self):
        tm = Tinymovr(node_id=1)
        tm.connect()
        time.sleep(0.5)
        state = tm.state
        self.assertEqual(state[0], 0, "Error flag present")
        motor_info = tm.motor_info
        self.assertEqual(motor_info[0], 1, "Motor not calibrated")
        if state[1] != 2:
            tm.set_state(2, 2)
        time.sleep(0.1)
        prev_pos = tm.encoder_estimates[0]
        counter = 0
        while True:
            pos = tm.encoder_estimates[0]
            # Periodically report iteration
            if counter % 1000 == 0:
                print(counter)
            # Report errors in received position values
            if (abs(prev_pos - pos) > 9000):
                print(str(counter) + ": " + str(pos) + ", " + str(prev_pos))
                pos = prev_pos
            # self.assertLess(abs(prev_pos - pos), 10000, "Pos threshold exceeded")
            tm.set_pos_setpoint(pos + random.uniform(-7000, 7000))
            prev_pos = pos
            counter += 1
            time.sleep(random.uniform(0.01, 0.1))

if __name__ == '__main__':
    unittest.main()