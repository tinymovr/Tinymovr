import can
import logging


def get_bus_config(suggested_type=None):
    configs = can.interface.detect_available_configs()
    if "auto" == suggested_type or None == suggested_type:
        config = configs[0]
        return config["interface"], config["channel"]
    else:
        for config in configs:
            if suggested_type == config["interface"]:
                return config["interface"], config["channel"]


def configure_logging():
    logging.getLogger("can.io.logger").setLevel(logging.WARNING)
    logging.getLogger("parso").setLevel(logging.WARNING)
    logging.getLogger("asyncio").setLevel(logging.WARNING)
    logger = logging.getLogger("tinymovr")
    logger.setLevel(logging.DEBUG)
    return logger
