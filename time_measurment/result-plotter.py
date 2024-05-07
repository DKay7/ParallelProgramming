import matplotlib.pyplot as plt

with open("time-experiment.txt") as f:
  data = f.read()

data = data.strip().split("\n")

X = []
Y = []

for num in data:
  X.append( float(num.split()[0]) )
  Y.append( float(num.split()[1]) )



plt.plot(X,Y)
plt.xlabel('Message size')
plt.ylabel('Average time')
plt.title('The dependence of the switching time between two nodes on the size of the message')
plt.show()
