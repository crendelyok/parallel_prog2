#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ISIZE 4000
#define JSIZE 4000

int main()
{
	// MPI init
	int size, rank;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	double starttime = MPI_Wtime();
	
	// Giving tasks
	int rows[ISIZE];
	int actual_rows[ISIZE];
	for (int i = ISIZE - 1; i >= 0; --i) {
		rows[i] = i % size;
	}	
	
	int this_size = ISIZE;
	double* a = NULL;
	if (rank == 0) a = calloc(ISIZE * JSIZE, sizeof(double));
	else a = calloc(this_size * ISIZE, sizeof(double)); 

	this_size = 0;
	for (int i = 0; i < ISIZE; ++i) {
		if (rows[i] == rank) this_size++;
	}
	//printf("pid %d: this_size: %d \n", rank, this_size);
	
	int local_rows_counter = this_size - 1;
	for (int i = ISIZE - 1; i >= 0; --i) {
		if (rows[i] == rank)
			actual_rows[local_rows_counter--] = i;
	} 	
	int i, j;
	
	double *buffer = calloc(JSIZE, sizeof(double));
	int sent = 0;
	MPI_Request* req = calloc(1, sizeof(MPI_Request));
	MPI_Status *status = calloc(1, sizeof(MPI_Status));
	for (i = this_size - 1; i >= 0; --i) {
		if (actual_rows[i] >= ISIZE - 3) {
			if (rank != 0)
				for (j = 0; j < JSIZE; ++j)
					*(a + i * JSIZE + j) = 10 * actual_rows[i] + j;
			else
				for (j = 0; j < JSIZE; ++j)
					*(a + actual_rows[i] * JSIZE + j) = 10 * actual_rows[i] + j;	
			
		}
		else {
			if ((rank + 3) % size != rank) { //size != 3 and size != 1
				int waiting_rank = (rank + size + size - 3);
				waiting_rank = waiting_rank % size;
				MPI_Recv(buffer, JSIZE, MPI_DOUBLE, waiting_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
				if (rank != 0) {
					for (j = 0; j < 2; ++j) 
						*(a + i * JSIZE + j) = 10 * actual_rows[i] + j;
					for (j = 2; j < JSIZE; ++j)
						*(a + i * JSIZE + j) = sin(0.00001 * buffer[j - 2]);		
				}
				else { //rank == 0
					for (j = 0; j < 2; ++j) 
						*(a + actual_rows[i] * JSIZE + j) = 10 * actual_rows[i] + j;
					for (j = 2; j < JSIZE; ++j)
						*(a + actual_rows[i] * JSIZE + j) = sin(0.00001 * buffer[j - 2]);		

				}
			}
			else { 
				if (rank != 0) {
					for (j = 0; j < 2; ++j) 
						*(a + i * JSIZE + j) = 10 * actual_rows[i] + j;
					for (j = 2; j < JSIZE; ++j)
						*(a + i * JSIZE + j) = sin(0.00001 * (*(a + (i+1)*JSIZE + (j-2))));		
				}
				else { //rank == 0
					for (j = 0; j < 2; ++j) 
						*(a + actual_rows[i] * JSIZE + j) = 10 * actual_rows[i] + j;
					for (j = 2; j < JSIZE; ++j)
						*(a + actual_rows[i] * JSIZE + j) = sin(0.00001 * (*(a + (actual_rows[i]+3)*JSIZE + (j-2))));
				}
			}
		}

		if (size == 2) {
		if ((rank + 3) % size != rank && actual_rows[i] - 3 >= 0) {
			if (sent) {
				MPI_Wait(req, status);
				sent = 0;
			}
			if (rank != 0) {
				MPI_Isend((a + i * JSIZE), JSIZE, MPI_DOUBLE, (rank + 3) % size, 0, MPI_COMM_WORLD, req);
				sent = 1;
			}
			else {
				MPI_Isend((a + actual_rows[i] * JSIZE), JSIZE, MPI_DOUBLE, (rank + 3) % size, 0, MPI_COMM_WORLD, req);
				sent = 1;
			}	
		}
		}
		else{
		if ((rank + 3) % size != rank && actual_rows[i] - 3 >= 0) {
			if (rank != 0) {
				MPI_Send((a + i * JSIZE), JSIZE, MPI_DOUBLE, (rank + 3) % size, 0, MPI_COMM_WORLD);
			}
			else {
				MPI_Send((a + actual_rows[i] * JSIZE), JSIZE, MPI_DOUBLE, (rank + 3) % size, 0, MPI_COMM_WORLD);
			}	
		}
		}
			
	}
		
	// Sending
	if (rank != 0) {
		for (i = 0; i < this_size; ++i)
			MPI_Send(a + i*JSIZE, JSIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
	// Collecting
	else
	{
		for (i = 0; i < ISIZE; ++i) {
			if (rows[i] == 0) continue;
			MPI_Recv((a + i * JSIZE), JSIZE, MPI_DOUBLE, rows[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
	
	free(req);
	free(status);
	free(a);
	free(buffer);
	MPI_Finalize();
	return 0;
}
