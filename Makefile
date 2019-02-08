all: benchmarkA benchmarkB

benchmarkA: benchmark-A.c
	cc -o benchmark-A benchmark-A.c;
benchmarkB: benchmark-B
		cc -o benchmark-B benchmark-B.c

clean:
	rm -f benchmark-A benchmark-B
