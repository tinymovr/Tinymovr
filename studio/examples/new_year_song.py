
import sys
import signal
from time import sleep
import can
from tinymovr import Tinymovr
from tinymovr.iface import IFace
from tinymovr.iface.can import CAN, guess_channel


tm1: Tinymovr = None

intensity = 9000

notes = {
    'A': 440,
    'B': 494,
    'C': 523,
    'D': 587,
    'E': 659,
    'F': 698,
    'G': 783,
    'A5': 880,
    'Bb5': 932
}

tune = [
    ('C', 1, intensity, 0),
    ('F', 1, intensity, 8000),
    ('F', 0.5, intensity, 8000),
    ('G', 0.5, intensity, 0),
    ('F', 0.5, intensity, 0),
    ('E', 0.5, intensity, 0),
    ('D', 0.5, intensity, -8000),
    ('D', 0.5, 0, -8000),
    ('D', 0.5, intensity, -8000),
    ('D', 0.5, 0, -8000),
    ('D', 1, intensity, 0),
    ('G', 1, intensity, 8000),
    ('G', 0.5, intensity, 8000),
    ('A5', 0.5, intensity, 8000),
    ('G', 0.5, intensity, 0),
    ('F', 0.5, intensity, 0),
    ('E', 1, intensity, 8000),
    ('C', 0.5, intensity, -8000),
    ('C', 0.5, 0, -8000),
    ('C', 0.5, intensity, 8000),
    ('C', 0.5, 0, 8000),
    ('A5', 0.9, intensity, 0),
    ('A5', 0.1, 0, -50000),
    ('A5', 0.5, intensity, 00),
    ('Bb5', 0.5, intensity, -2000),
    ('A5', 0.5, intensity, -4000),
    ('G', 0.5, intensity, -8000),
    ('F', 1, intensity, -16000),
    ('D', 1, intensity, 0),
    ('C', 0.4, intensity, -10000),
    ('C', 0.1, 0, 50000),
    ('C', 0.4, intensity, 10000),
    ('C', 0.1, 0, 0),
    ('D', 1, intensity, -32000),
    ('G', 1, intensity, -80000),
    ('E', 1, intensity, -32000),
    ('F', 0.5, intensity, 8000),
    ('F', 0.5, intensity, 16000),
    ('F', 0.5, intensity, 32000),
    ('F', 0.5, intensity, 80000),
]


def main():
    global tm1
    
    channel = guess_channel(bustype_hint='slcan')
    can_bus: can.Bus = can.Bus(bustype='slcan',
                               channel=channel,
                               bitrate=1000000)
    iface: IFace = CAN(can_bus)
    tm1 = Tinymovr(node_id=1, iface=iface)

    if tm1.motor_info.calibrated == 0:
        tm1.calibrate()
        sleep(10)
        assert(tm1.motor_info.calibrated == 1)

    tm1.set_limits(300000, 10)

    sleep(0.1)

    tm1.set_vel_setpoint(0)
    tm1.set_cur_setpoint(0, 0, 0)

    sleep(0.1)

    tm1.velocity_control()

    sleep(0.1)

    for t in tune:
        freq = notes[t[0]]
        tm1.set_vel_setpoint(t[3])
        tm1.set_cur_setpoint(0, freq, t[2])
        sleep(t[1] * 0.5)

    tm1.idle()


def signal_handler(signum, frame):
    # ignore additional signals
    signal.signal(signum, signal.SIG_IGN) 
    # disarm
    disarm()
    # exit
    sys.exit(0)


def disarm():
    global tm1, tm2
    tm1.idle()
    tm2.idle()


if __name__ == "__main__":
    # register the signal with the signal handler first
    signal.signal(signal.SIGINT, signal_handler)
    try:
        main()
    except:
        disarm()
        raise