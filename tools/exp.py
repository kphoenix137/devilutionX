import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

# Define the exponential function to be fitted
def exponential_func(x, a, b, c):
    return a * np.exp(b * x) + c

# Define the data
x = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50])
y = np.array([0, 2000, 4620, 8040, 12489, 18258, 25712, 35309, 47622, 63364, 83419, 108879, 141086, 181683, 231075, 313656, 424067, 571190, 766569, 1025154, 1366227, 1814568, 2401895, 3168651, 4166200, 5459523, 7130496, 9281874, 12042092, 15571031, 20066900, 25774405, 32994399, 42095202, 53525811, 67831218, 85670061, 107834823, 135274799, 169122009, 210720231, 261657253, 323800420, 399335440, 490808349, 601170414, 733825617, 892680222, 1082908612, 1310707109, 1583495809])

# Fit the exponential function to the data
popt, pcov = curve_fit(exponential_func, x, y)

# Plot the data and the fit
plt.plot(x, y, 'bo', label='Data')
plt.plot(x, exponential_func(x, *popt), 'r', label='Fit')
plt.legend()
plt.xlabel('x')
plt.ylabel('y')
plt.show()
