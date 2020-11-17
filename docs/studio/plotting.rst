******************
Plotting in Studio
******************

Overview
########

Tinymovr Studio features a capable and fast plotter to visualize your setup in real time. The plotter is accessible from within the IPython terminal that hosts Tinymovr Studio.


Example: Plotting Encoder Estimates
###################################

Let us imagine that we want to plot the position and velocity estimates of our encoder. The following will do the trick:

.. code-block:: python

    plot(lambda: [tm1.encoder_estimates])

A plot window will show up on screen. Notice that both values (position and velocity) are plotted. This is because we passed the endpoint itself as an argument. The plotter is smart enough to know to expand the returned dictionary, and assign values to the correct keys.

Also note that there are two y-axes in the plot, one on the left and one on the right. Each of these corresponds to one value being plotted, and will adjust to that value's range. You can have as many values as you wish, the plotter will add so-called 'parasite' axes on the right side of the plot for each value. However with more than three or four it doesn't really look pretty...


Plotting values from multiple endpoints
#######################################

It is possible to plot multiple endpoints from the same or multiple devices with the same syntax:

.. code-block:: python

    plot(lambda: [tm1.encoder_estimates, tm1.setpoints])

In the above example, estimates are plotted together with setpoints for position and velocity.


Multiple Windows
################

(to be properly implemented)


Performance
###########

The plotter uses Matplotlib's animation api with blitting to make drawing on screen particularly fast. To maintain fast drawing, the chart is only rescaled to fit the data periodically, as needed. In general, the bottleneck in plotting should not be drawing, unless you have an extremely high number of plot values.
