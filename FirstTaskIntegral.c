#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

double f(double x) {
	return sqrt(4 - x * x) ;
}
int main(int argc, char* argv[]) {
	int N;
	if (argc > 1) {
		N = atoi(argv[1]);
	}
	else {
		printf("Not arguments\n");
		return 1;
	}
	int rc;
	rc = MPI_Init(&argc, &argv);
	

	if (rc != MPI_SUCCESS) {
		printf("Error starting MPI program. Terminating");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	
	MPI_Status status;
	int NumTasks, Rank;
	double a = 0, b = 2;
	double I, I_k, x_i;
	double h = (b - a) / (double)N;
	MPI_Comm_size(MPI_COMM_WORLD, &NumTasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
	double StartTimeParallel, StartTimeSequential;
	double EndTimeParallel, EndTimeSequential, SpeedUp;
	StartTimeParallel = MPI_Wtime();
	I_k = 0;
	for (int i = Rank; i <= N; i += NumTasks) {
		x_i = a + i * h;
		if (i == 0 || i == N){
			I_k += h * f(x_i) / 2;
		} else {
			I_k += h * f(x_i);
		}
	}
	if (Rank == 0) {

		I = I_k;
		for(int i = 1; i < NumTasks; i++) {
			MPI_Recv(&I_k, 1, MPI_DOUBLE, i, 2,  MPI_COMM_WORLD, &status);
			I += I_k;
		}
		EndTimeParallel = MPI_Wtime();
		printf("Number of processes = %d\nNumber of segment splits = %d\n", NumTasks, N);
		printf("Parallel result = %.6lf\n", I);
		printf("Parallel algorithm Time = %lf\n", EndTimeParallel - StartTimeParallel);
		StartTimeSequential = MPI_Wtime();
		I = 0;
		for(int i = 0; i <= N; i++){
			x_i = a + i * h;
			if (i == 0 || i == N){
                        	I += h * f(x_i) / 2;
                	} else {
                        	I += h * f(x_i);
                	}
		}
		EndTimeSequential = MPI_Wtime();
		printf("Sequential result = %.6lf\n", I);
		printf("Sequential algorithm Time = %lf\n", EndTimeSequential - StartTimeSequential);
		SpeedUp = (EndTimeSequential - StartTimeSequential) / (EndTimeParallel - StartTimeParallel);
		printf("SpeedUp = %lf\n", SpeedUp);
	} else {
		MPI_Send(&I_k, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
	}		
	MPI_Finalize();
	return 0;
}

