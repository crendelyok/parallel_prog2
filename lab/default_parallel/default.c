#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ISIZE 1000
#define JSIZE 1000

// parallel version

int main()
{
	// MPI init
	int size, rank;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	double starttime = MPI_Wtime();

	// Giving tasks
	int this_size = ISIZE / size;
	int rest_size = ISIZE % size;
	if (rank < rest_size) this_size++;
	int l_index = rank * this_size;
	if (rank > rest_size) l_index += rest_size;
	if (rank <= rest_size) l_index += rank;

	double* a = NULL;
	if (rank == 0) a = calloc(ISIZE * JSIZE, sizeof(double));
	else a = calloc(this_size * JSIZE, sizeof(double)); 
	printf("pid %d, size = %d\n", rank, this_size);


	// Parallel part
	int i, j;
	for (i = 0; i < this_size; i++) 
	{
		for (j = 0; j < JSIZE; j++)
		{
			*(a + i*JSIZE + j) = 10 * (i + l_index) + j;
		}
	}

	for (i = 0; i < this_size; i++) 
	{
		for (j = 0; j < JSIZE; j++)
		{
			*(a + i*JSIZE + j) = sin(0.00001 * (*(a + i*JSIZE + j)));
		}
	}

	if (rank != 0) {
		MPI_Send(a, this_size * JSIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		// Collecting
		int collected = 0;
		while(collected < size - 1) 
		{
			// Giving tasks
			int waiting_rank = collected + 1;
			int waiting_size = ISIZE / size;
			int l_index = waiting_rank * waiting_size;
			int waiting_rest_size = ISIZE % size;
			if (waiting_rank < waiting_rest_size) waiting_size++;
			if (waiting_rank > waiting_rest_size) l_index += waiting_rest_size;
			if (waiting_rank <= waiting_rest_size) l_index += waiting_rank;
			//int r_index = l_index + waiting_size;
			MPI_Recv(a +  l_index * JSIZE, waiting_size * JSIZE,
				 MPI_DOUBLE, waiting_rank, 0, MPI_COMM_WORLD,
				 MPI_STATUS_IGNORE);
			collected ++;
		}	
		// Writing
		FILE *ff;
		ff = fopen("result_default_parallel.txt", "w+");
		for (i = 0; i < ISIZE; i++) 
		{
			for (j = 0; j < JSIZE; j++)
			{
				fprintf(ff, "%f ", *(a + i*ISIZE + j));
			}
			fprintf(ff, "\n");
		}
		fclose(ff);
		printf("elapsed time: %f\n", MPI_Wtime() - starttime);
	}
	MPI_Finalize();
	return 0;
}
