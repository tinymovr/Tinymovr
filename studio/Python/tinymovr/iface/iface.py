
from tinymovr.codec import Codec
from typing import Dict


class IFace:

    def get_codec(self) -> Codec:
        raise NotImplementedError()

    def get_ep_map(self) -> Dict:
        raise NotImplementedError()

    def send(self, node_id: int, endpoint_id: int, payload: bytearray):
        raise NotImplementedError()

    def receive(self, node_id: int, endpoint_id: int, timeout: float=0.05):
        raise NotImplementedError()
