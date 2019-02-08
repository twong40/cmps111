all: benchmark

benchmark: benchmark.c
	cc -o argshell benchmark.c

clean:
	rm -f benchmark
