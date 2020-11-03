import math
import numbers
from collections import ChainMap
from typing import Callable, List

import matplotlib
try:
    matplotlib.use('TkAgg')
    import matplotlib.pyplot as plt
except ImportError:
    # Used for headless CI
    matplotlib.use('Agg')
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
    ax.set_xlabel('Samples')
    fig.set_size_inches(12, 6)
    fig.canvas.set_window_title('Tinymovr Plotter')
    ax.margins(0, 0.3)

    xdata = range(sample_count)
    try:
        ydata = {k: [v.m]*sample_count for k, v in flat_dict.items()}
    except AttributeError:
        ydata = {k: [v]*sample_count for k, v in flat_dict.items()}

    pars = [ax.twinx() for i in range(len(flat_dict) - 1)]
    pars.insert(0, ax)

    for par in pars:
        par.margins(0, 0.3)

    if len(pars) >= 3:
        for i in range(len(pars) - 2):
            pars[i+2].spines["right"].set_position(("axes", 1.12 + 0.12 * i))
            make_patch_spines_invisible(pars[i+2])
            pars[i+2].spines["right"].set_visible(True)

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
            try:
                ydata[k].append(new_data[k].m)
            except AttributeError:
                ydata[k].append(new_data[k])
            ydata[k].pop(0)
            lines[i].set_ydata(ydata[k])
            ymin_val, ymax_val = min(ydata[k]), max(ydata[k])
            if ( (ymin_val < ymin_plot or ymax_val > ymax_plot) or
                 (ymax_val - ymin_val < (ymax_plot - ymin_plot) * 0.1 and
                 ymax_val - ymin_val > 1) ):
                redraw = True

        if redraw:
            for par in pars:
                par.relim()
                par.autoscale_view()
            fig.canvas.draw()
        return lines

    global ani
    ani = animation.FuncAnimation(fig, animate, interval=25, blit=True)

    ax.legend(lines, [line.get_label() for line in lines], loc='upper left')
    fig.tight_layout()
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
