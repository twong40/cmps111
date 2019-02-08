all: benchmark
	./ benchmark
	rm -f benchmark

benchmark: benchmark.c
	cc -o benchmark benchmark.c

clean:
	rm -f benchmark
