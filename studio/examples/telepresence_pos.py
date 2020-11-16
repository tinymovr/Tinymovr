
import sys
import signal
from time import sleep
import can
from tinymovr import Tinymovr
from tinymovr.iface.can import CAN, guess_channel


tm1: Tinymovr = None
tm2: Tinymovr = None


def main():
    global tm1, tm2
    
    channel = guess_channel(bustype_hint='slcan')
    can_bus: can.Bus = can.Bus(bustype='slcan',
                               channel=channel,
                               bitrate=1000000)
    iface: IFace = CAN(can_bus)
    tm1 = Tinymovr(node_id=1, iface=iface)
    tm2 = Tinymovr(node_id=2, iface=iface)

    assert(tm1.motor_info.calibrated == 1)
    assert(tm2.motor_info.calibrated == 1)

    tm1.set_limits(300000, 25)
    tm2.set_limits(300000, 25)

    tm1.set_gains(280, 1.1e-4)
    tm2.set_gains(280, 1.1e-4)

    sleep(0.1)

    tm1.position_control()
    tm2.position_control()

    sleep(0.1)

    offset_1 = tm1.encoder_estimates.position
    offset_2 = tm2.encoder_estimates.position

    while True:
        est_1 = tm1.encoder_estimates
        est_2 = tm2.encoder_estimates

        mean_pos = ((est_1.position - offset_1) + (est_2.position - offset_2)) / 2.0
        mean_vel = (est_1.velocity + est_2.velocity) / 2.0

        tm1.set_pos_setpoint(mean_pos + offset_1, mean_vel, 0)
        tm2.set_pos_setpoint(mean_pos + offset_2, mean_vel, 0)

        sleep(0.0005)


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