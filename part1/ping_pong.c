#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define STANDARD 0
#define SYNC 1
#define BUFF 2

int BUFF_SIZE = 1E7;
int MSG_COUNT = 1E3;
int MAX_MSG_SIZE = 1E7;

double sender(int size, int mode) {
    MPI_Barrier(MPI_COMM_WORLD);
    char *buff = malloc(size);
    int i;
    double start = MPI_Wtime();
    for (i = 0; i < MSG_COUNT; i++) {
	
	if (mode == STANDARD) {
	    MPI_Send(buff, size, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buff, size, MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	} else if (mode == SYNC) {
            MPI_Ssend(buff, size, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buff, size, MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else if (mode == BUFF) {
            MPI_Bsend(buff, size, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buff, size, MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    double end = MPI_Wtime();
    free(buff);
    return end - start;
}

void receiver(int size, int mode) {
    MPI_Barrier(MPI_COMM_WORLD);
    char *buff = malloc(size);
    int i;
    for (i = 0; i < MSG_COUNT; i++) {
        if (mode == STANDARD) {
            MPI_Recv(buff, size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(buff, size, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
	} else if (mode == SYNC) {
            MPI_Recv(buff, size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Ssend(buff, size, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        } else if (mode == BUFF) {
           MPI_Recv(buff, size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
           MPI_Bsend(buff, size, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        }
    }
    free(buff);
}

double ping_pong(int rank, int size, int mode) {
    if (rank == 0) {
        double time = sender(size, mode);
        return time;
    } else if (rank == 1) {
        receiver(size, mode);
    }
    return 0;
}


int main(int argc, char *argv[]) {

    MPI_Init(NULL, NULL);
    
    int world_rank, mode;
    mode = atoi(argv[1]); // 0 - standard, 1 - sync, 2 - buff
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Buffer_attach(malloc(BUFF_SIZE), BUFF_SIZE);
        
    for (int size = 1; size < MAX_MSG_SIZE; size *= 2) { // in bytes
        double time = ping_pong(world_rank, size, mode); // in seconds
	if (world_rank == 0) {
            if (size == 1) {
                printf("TIME: %.9f\n", time); // in seconds
            }
	    double x = MSG_COUNT * 8 * (size / 10E6); // Mbit
            printf("%d %d %.9f\n", mode, size, x / (time / 2)); // Mbit/s
        }
    }
    MPI_Finalize();
}
