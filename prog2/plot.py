from scipy.interpolate import make_interp_spline
import matplotlib.pyplot as plt
import numpy as np
import csv
import sys

x, y = [],[]

with open(sys.argv[1], 'r') as csvfile:
	lines = csv.reader(csvfile, delimiter=',')
	for row in lines:
		x.append(int(row[0]))
		y.append(float(row[1]))

x2y = {}
for i, xi in enumerate(x):
    if xi in x2y:
        x2y[xi][0] += y[i]
        x2y[xi][1] += 1
    else:
        x2y[xi] = [y[i], 1]

x, y= [],[]
for k,v in x2y.items():
    x.append(k)
    y.append(v[0]/v[1])

# X_Y_Spline = make_interp_spline(x, y)
# X_ = np.linspace(min(x), max(x), 500)
# Y_ = X_Y_Spline(X_)
# plt.plot(X_, Y_, color = 'b', linestyle = 'solid',
# 		marker = '',label = "Writer delay")

plt.plot(x, y, color = 'b', linestyle = 'solid',
		marker = 'o',label = "Cache Miss Rate (%)")

plt.xlabel('ELements')
plt.ylabel('DCA')
plt.title('Elements vs Cache Miss Rate(%)', fontsize = 20)
plt.grid()
plt.legend()
plt.show()