import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv('results.csv')
n = data['n']
time = data['time'] 

plt.plot(n, time[0]/time)
plt.xlabel('n')
plt.ylabel('time')
plt.title('n(time)')
plt.show()
