#include <omp.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

int main(int argc, char *argv[]) {
	assert(argc == 2);

        char *endptr, *str;
        long val;
        str = argv[1];
        val = strtol(str, &endptr, 10);

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                   || (errno != 0 && val == 0)) {
                perror("strtol");
        	exit(EXIT_FAILURE);
       	}

        if (endptr == str) {
        	fprintf(stderr, "No digits were found\n");
                exit(EXIT_FAILURE);
        }
	printf("Input number N is: %ld\n", val);
	int n_threads = omp_get_max_threads();

	double ans[n_threads];
	for (int i = 0; i < n_threads; ++i) ans[i] = 0;

	#pragma omp parallel for 
	for (long i = 1; i <= val; ++i)
	{	
		int thr_num = omp_get_thread_num();
		ans[thr_num] += (double)(1 / (double)i);
		//printf("%d : %d : %f \n", thr_num, i, ans[thr_num]);
	}

	//printf("\n\n\n");
	double result = 0;
	for (int i = 0; i < n_threads; ++i){
		result += ans[i];
		//printf("%f ", ans[i]);
	}
	printf("\n%f\n", result);
	return 0;
}


// could used:
// double ans = 0;
// #pragma omp parallel for reduction(+:ans)
// for (...) sum += i;
// printf(sum);
