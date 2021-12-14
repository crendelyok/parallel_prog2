#include <omp.h>
#include <stdio.h>

int main() {

#pragma omp parallel
{
	printf("Thread number %d ", omp_get_thread_num());
	printf("Hello world!\n");
}
	return 0;
}
