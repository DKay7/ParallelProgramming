import os
import csv
from tqdm import tqdm

os.system("rm integral.out")
os.system("rm results.csv")

filename = 'results.csv'
if not os.path.exists(filename):
    open(filename, 'w').close()

with open(filename, 'w', newline='') as csvfile:
    fieldnames = ['n', 'time']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()

# os.system("mpicc -O3  main.c -o build/task.out")
os.system("clang -o integral.out integral.c -lm")
for param in tqdm(range(1, 1800)):
    os.system(f"./integral.out {param} 0.000001 > /dev/null")