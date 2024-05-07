
CXX := mpic++
CC := mpicc

main: main.o functions.o
	$(CXX) $^ -o main

.PHONY: run
run: main
	mpirun -np 3 main
	python3 plot-solution.py

.PHONY: clean
clean:
	rm main main.o functions.o *.csv