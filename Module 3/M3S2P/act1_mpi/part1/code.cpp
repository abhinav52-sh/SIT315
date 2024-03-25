/*
 * Name: Abhinav Sharma
 * Student ID: 2210994752
 * Unit: SIT315
 */
 
#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
  int world_size, world_rank;
  char message[13] = "Hello World!"; // 12 characters + null terminator

  MPI_Init(&argc, &argv); // Initialize MPI

  MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Get number of processes
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Get rank of this process

  if (world_rank == 0) { // Master process
    for (int i = 1; i < world_size; ++i) {
      MPI_Send(message, strlen(message) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
  } else { // Worker process
    MPI_Recv(message, strlen(message) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Worker %d received: %s\n", world_rank, message);
  }

  MPI_Finalize(); // Finalize MPI

  return 0;
}

