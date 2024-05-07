
CXX := mpic++
CC := mpicc

main: main.o
	$(CXX) main.o -o main

.PHONY: run
run: main
	mpirun -np 2 main
	python3 result-plotter.py

.PHONY: clean
clean:
	rm main main.o time-experiment.txt