CAN_EP_BITS = 6


class ResponseError(Exception):
    def __init__(self, kw, *args, **kwargs):
        msg = "Node {} did not respond".format(kw)
        super().__init__(msg, *args, **kwargs)
        self.kw = kw


class CANBusChannel:
    def __init__(self, node_id, bus):
        self.node_id = node_id
        self.bus = bus

    def send(self, data, ep_id):
        self.make_arbitration_id(ep_id)

    def recv(self, ep_id):
        self.make_arbitration_id(ep_id)

    def make_arbitration_id(self, endpoint_id: int) -> int:
        """
        Generates a CAN id from node and endpoint ids
        """
        return (self.node_id << CAN_EP_BITS) | endpoint_id
