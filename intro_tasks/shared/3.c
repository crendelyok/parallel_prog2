#include <omp.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

int main(int argc, char *argv[]) {
	int shared = 0;
	#pragma omp parallel for ordered 
	for (int i = 0; i < omp_get_num_threads(); ++i)
	{	
		#pragma omp ordered
		{
			int thr_num = omp_get_thread_num();
			printf("thr_num: %d // %d\n", thr_num, ++shared);
		}
	}
	return 0;
}
