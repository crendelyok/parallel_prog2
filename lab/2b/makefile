out_2b.out: 2b.c
	mpicc -Wall -Wextra -std=c99 2b.c -o out_2b.out -lm
	qsub j_2b
clean:
	rm result_*
