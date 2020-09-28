from collections import ChainMap
from typing import Callable, List

import matplotlib.pyplot as plt
import matplotlib.animation as animation

from flatten_dict import flatten

sample_count = 500


def plot(getter: Callable):

    flat_dict = chain_and_flatten(getter())

    fig, ax = plt.subplots()

    ax.margins(0, 0.3)

    xdata = range(sample_count)
    ydata = {k: [flat_dict[k]]*sample_count for k in flat_dict}

    lines = [ax.plot(xdata, ydata[k], color='C'+str(i), label=k, lw=2)[0]
             for i, k in enumerate(flat_dict)]

    def animate(i) -> List:
        new_data = chain_and_flatten(getter())

        _, _, ymin_plot, ymax_plot = plt.axis()
        redraw = False

        for i, k in enumerate(new_data):
            ydata[k].append(new_data[k])
            ydata[k] = ydata[k][-sample_count:]
            lines[i].set_ydata(ydata[k])
            ymin_val, ymax_val = min(ydata[k]), max(ydata[k])
            if ymin_val < ymin_plot or ymax_val > ymax_plot:
                redraw = True

        if redraw:
            ax.relim()
            ax.autoscale_view()
            plt.draw()
        return lines

    ani = animation.FuncAnimation(fig, animate, interval=25, blit=True)

    ax.relim()
    ax.autoscale_view()
    plt.legend(loc='upper left')
    plt.show()


def chain_and_flatten(dicts):
    return flatten(dict(ChainMap(*dicts)), reducer='path')
