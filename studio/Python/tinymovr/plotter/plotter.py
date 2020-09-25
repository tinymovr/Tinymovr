import datetime as dt
from types import LambdaType
from typing import Callable

import matplotlib.pyplot as plt
import matplotlib.animation as animation

from flatten_dict import flatten

class Plotter:

    def __init__(self, getter: Callable):
        self.getter = getter

        # Create figure for plotting
        fig = plt.figure()
        ax = fig.add_subplot(1, 1, 1)
        xs = []
        ys_by_label = {}

        # This function is called periodically from FuncAnimation
        def animate(i, xs, ys_by_label):

            flat_values = flatten(self.getter(), reducer='path')
            for k, v in flat_values:
                try:
                    ys_by_label[k].append(v)
                    ys_by_label[k] = ys_by_label[k][-20:]
                except KeyError:
                    ys_by_label[k] = [v]

            xs.append(dt.datetime.now().strftime('%H:%M:%S.%f'))
            xs = xs[-20:]

            # Draw x and y lists
            ax.clear()
            for k, ys in ys_by_label:
                ax.plot(xs, ys, color='blue', label=k)

            # Format plot
            plt.xticks(rotation=45, ha='right')
            plt.subplots_adjust(bottom=0.30)
            plt.title('Time')
            plt.ylabel('Values')

            plt.legend()

        # Set up plot to call animate() function periodically
        ani = animation.FuncAnimation(fig, animate, fargs=(xs, ys_by_label), interval=100)
        plt.show()
