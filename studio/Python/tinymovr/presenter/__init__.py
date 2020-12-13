from tinymovr.presenter.dict_obj import dict_obj
from tinymovr.presenter.presenters import present_default, present_error

presenter_map = {
    "default": present_default,
    "error": present_error
}