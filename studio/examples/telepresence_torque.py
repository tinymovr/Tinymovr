
import sys
import signal
from time import sleep
import can
from tinymovr import Tinymovr
from tinymovr.iface.can import CAN, guess_channel
from tinymovr.units import get_registry


tm1: Tinymovr = None
tm2: Tinymovr = None


ureg = get_registry()
A = ureg.ampere
s = ureg.second
tick = ureg.tick


def main():
    global tm1, tm2
    
    channel = guess_channel(bustype_hint='slcan')
    can_bus: can.Bus = can.Bus(bustype='slcan',
                               channel=channel,
                               bitrate=1000000)
    iface: IFace = CAN(can_bus)
    tm1 = Tinymovr(node_id=1, iface=iface)
    tm2 = Tinymovr(node_id=2, iface=iface)

    assert(tm1.motor_config.flags == 1)
    assert(tm2.motor_config.flags == 1)

    tm1.set_limits(200000, 15)
    tm2.set_limits(200000, 15)

    sleep(0.1)

    tm1.current_control()
    tm2.current_control()

    sleep(0.1)

    offset_1 = tm1.encoder_estimates.position
    offset_2 = tm2.encoder_estimates.position

    while True:
        est_1 = tm1.encoder_estimates
        est_2 = tm2.encoder_estimates
        p_1 = est_1.position - offset_1
        p_2 = est_2.position - offset_2
        v_1 = est_1.velocity
        v_2 = est_2.velocity

        Iq_1 = (3e-3 * (p_2 - p_1) * (A/tick) + 
                5e-5 * (v_2 - v_1) * (A*s/tick))
        Iq_2 = (3e-3 * (p_1 - p_2) * (A/tick) + 
                5e-5 * (v_1 - v_2) * (A*s/tick))
        
        tm1.set_cur_setpoint(Iq_1)
        tm2.set_cur_setpoint(Iq_2)

        sleep(0.0001)


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