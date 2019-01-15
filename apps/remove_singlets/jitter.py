import numpy as np
import math
def jitter(x, factor=1, amount=None):
    """Jitters input array

    :param x: an array of numbers which you want to jitter
    :param factor: jitter factor
    :param amount: scale/spread
    :returns: jittered array
    :rtype: np.array

    """
    z = np.max(x) - np.min(x)
    if z == 0:
       z = np.min(x)
    if z == 0:
       z = 1
    if amount is None:
        xx = np.unique(np.sort(np.round(x, 3 - math.floor(math.log10(z)))))
        d = np.diff(xx)
        if (len(d) > 0):
            d = np.min(d)
        elif (xx != 0):
            d = xx/10
        else:
            d = z/10
        amount = factor/5 * np.abs(d)
    elif amount == 0:
        amount = factor * (z/50)

    x = x + np.around(np.random.normal(0, amount, len(x)), decimals=2)
    return x
