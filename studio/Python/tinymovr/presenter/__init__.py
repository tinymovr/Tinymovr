from tinymovr.presenter.dict_obj import DictObj
from tinymovr.presenter.state_obj import StateObj
from tinymovr.presenter.presenters import present_default, present_state, strip_end

presenter_map = {
    "default": present_default,
    "state": present_state
}