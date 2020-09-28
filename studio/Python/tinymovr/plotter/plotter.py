import math
import numbers
from collections import ChainMap
from typing import Callable, List

import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
import matplotlib.animation as animation

from flatten_dict import flatten

sample_count = 500

ani = None

# Multi-axes (parasitic) are based on the following example:
# https://matplotlib.org/3.1.1/gallery/ticks_and_spines/multiple_yaxis_with_spines.html


def plot(getter: Callable):

    plt.ion()

    flat_dict = chain_and_flatten(getter())
    fig, ax = plt.subplots()
    fig.canvas.set_window_title('Tinymovr Plotter')
    ax.margins(0, 0.3)

    xdata = range(sample_count)
    ydata = {k: [flat_dict[k]]*sample_count for k in flat_dict}

    pars = [ax.twinx() for i in range(len(flat_dict) - 1)]
    pars.insert(0, ax)

    for par in pars:
        par.margins(0, 0.3)

    if len(pars) >= 3:
        for i in range(len(pars) - 2):
            pars[i].spines["right"].set_position(("axes", math.pow(1.2, i+1)))
            make_patch_spines_invisible(pars[i])
            pars[i].spines["right"].set_visible(True)

    lines = [z[1].plot(xdata, ydata[z[0]], color='C'+str(i), label=z[0], lw=2)[0]
             for i, z in enumerate(zip(flat_dict, pars))]

    for par, line in zip(pars, lines):
        par.set_ylabel(line.get_label())
        par.yaxis.label.set_color(line.get_color())
        par.tick_params(axis='y', colors=line.get_color())

    def animate(i) -> List:
        new_data = chain_and_flatten(getter())
        redraw = False

        for i, z in enumerate(zip(flat_dict, pars)):
            k = z[0]
            par = z[1]
            _, _, ymin_plot, ymax_plot = par.axis()
            ydata[k].append(new_data[k])
            ydata[k] = ydata[k][-sample_count:]
            lines[i].set_ydata(ydata[k])
            ymin_val, ymax_val = min(ydata[k]), max(ydata[k])
            if ymin_val < ymin_plot or ymax_val > ymax_plot:
                redraw = True

        if redraw:
            for par in pars:
                par.relim()
                par.autoscale_view()
            plt.draw()
        return lines

    global ani
    ani = animation.FuncAnimation(fig, animate, interval=25, blit=True)

    ax.legend(lines, [line.get_label() for line in lines], loc='upper left')
    plt.show()


def chain_and_flatten(dicts):
    if isinstance(dicts, dict):
        d = dicts
    elif isinstance(dicts, list):
        d = {}
        for e in dicts:
            if isinstance(e, dict):
                d.update(e)
            elif isinstance(e, numbers.Number):
                d["val"] = e
    return flatten(d, reducer='path')


def make_patch_spines_invisible(ax):
    ax.set_frame_on(True)
    ax.patch.set_visible(False)
    for sp in ax.spines.values():
        sp.set_visible(False)
