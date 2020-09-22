import can


class Test(can.BusABC):

    def __init__(self, channel, can_filters=None, **kwargs):
        super().__init__(channel, can_filters, **kwargs)
        self.channel_info = "Test Channel"

    def send(self, msg: can.Message):
        print(msg)

    def _recv_internal(self, timeout) -> can.Message:
        return '', True
