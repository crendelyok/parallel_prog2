#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ISIZE 4000
#define JSIZE 10000

int main()
{
	// MPI init
	int size, rank;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	double starttime = MPI_Wtime();

	// Giving tasks
	int this_size = JSIZE / size;
	int rest_size = JSIZE % size;
	int sizes[28];
	for (int i = 0; i < size; ++i) {
		sizes[i] = this_size;
		if (i < rest_size) sizes[i]++; 
	}
	this_size = sizes[rank];

	int r_indexes[28];
	r_indexes[0] = JSIZE - sizes[0];
	for (int i = 1; i < size; ++i) {
		r_indexes[i] = r_indexes[i-1];
		r_indexes[i] -= sizes[i];
	}
	int r_index = r_indexes[rank];

	double* a = NULL;
	if (rank == 0) a = calloc(ISIZE * JSIZE, sizeof(double));
	else a = calloc((this_size + 1) * ISIZE, sizeof(double)); 
	printf("pid %d, size = %d, r_index = %d\n", rank, this_size, r_index);


	int i, j;	
	if (rank == 0) 
	{ 
		for (i = 0; i < ISIZE; ++i)
			*(a + i * JSIZE + JSIZE - 1) = 10 * i + JSIZE - 1;
		for (j = this_size - 1; j >= 0; --j) 
			*(a + r_index + j) = j + r_index;
		if (size != 1)
			MPI_Send((a + r_index), 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
		for (i = 1; i < ISIZE; ++i) {
			for (j = this_size - 2; j >= 0; --j) {
				//printf("qewq %f \n", *(a + (i-1) * JSIZE + r_index + (j+1)));
				*(a + i * JSIZE + r_index + j) = 
					sin(0.00001 * (*(a + (i-1)*JSIZE + r_index + (j+1))));
				
			}
			if (size != 1 && i != ISIZE-1)
				MPI_Send((a + i*JSIZE + r_index), 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
		}
	}
	else //rank != 0
	{
		for (j = 0; j < this_size; ++j) 
			*(a + j) = j + r_index; 
		if (rank + 1 != size)
			MPI_Send(a, 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);	
		for (i = 1; i < ISIZE; ++i) {
			double buffer = 0;
			//MPI_Recv((a + i * this_size + this_size - 1), 1, ...
			MPI_Recv(&buffer, 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			*(a + i * this_size + this_size - 1) = 
					sin(0.00001 * buffer);
			for (j = this_size-2; j >= 0; j--)
				*(a + i * this_size + j) = 
					sin(0.00001 * (*(a + (i-1)*this_size + (j + 1))));
			if (rank + 1 != size && i != ISIZE-1)
				MPI_Send((a + i*this_size), 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
		}
		
	}

	// Sending
	if (rank != 0) {
		for (i = 0; i < ISIZE; ++i)
			MPI_Send(a + i*this_size, this_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
	// Collecting
	else
	{
		int collected = 0;
		while(collected < size - 1) 
		{
			int waiting_rank = collected + 1;
			int waiting_size = sizes[waiting_rank];
			int waiting_r_index = r_indexes[waiting_rank];
			for (i = 0; i < ISIZE; ++i)
				MPI_Recv(a + i * JSIZE + waiting_r_index, waiting_size,
					 MPI_DOUBLE, waiting_rank, 0, MPI_COMM_WORLD,
 	      				 MPI_STATUS_IGNORE);
			collected ++;
		}	
		printf("elapsed time: %f\n", MPI_Wtime() - starttime);
		// Writing
		FILE *ff;
		ff = fopen("result_default_parallel.txt", "w+");
		for (i = 0; i < ISIZE; i++) 
		{
			for (j = 0; j < JSIZE; j++)
			{
				fprintf(ff, "%f ", *(a + i*JSIZE + j));
			}
			fprintf(ff, "\n");
		}
		fclose(ff);
	}
	MPI_Finalize();
	return 0;
}
