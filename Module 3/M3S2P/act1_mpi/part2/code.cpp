/*
 * Name: Abhinav Sharma
 * Student ID: 2210994752
 * Unit: SIT315
 */

#include <mpi.h>
#include <stdio.h>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    int numtasks, rank, name_len;
    char name[MPI_MAX_PROCESSOR_NAME];
    char message[100];  // Buffer for the message to broadcast

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of tasks/processes
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    // Get the rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Find the processor name
    MPI_Get_processor_name(name, &name_len);

    // Define the message to broadcast (only the master does this)
    if (rank == 0) {
        strcpy(message, "Hello, Sending a broadcast message from master process\n\n");
    }

    // Broadcast the message from the master process to all others
    MPI_Bcast(message, sizeof(message), MPI_CHAR, 0, MPI_COMM_WORLD);

    // Print the received message on all processes
    printf("Hello, Receiving a message from %s, rank %d out of %d. The broadcasted 
            message is: %s\n\n", name, rank, numtasks, message);

    // Finalize the MPI environment
    MPI_Finalize();

    return 0;
}
