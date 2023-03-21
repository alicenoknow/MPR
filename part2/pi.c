#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>


double get_coord() {
    double range = 1;
    double div = RAND_MAX / range;
    return (rand() / div);
}


int is_inside(double x, double y) {
    return pow(x, 2) + pow(y, 2) < 1;
}


unsigned long long calc_inside(unsigned long long n) {
    unsigned long long inside = 0;
    double x, y;

    for (unsigned long long i=0; i<n; i++) {
        x = get_coord();
        y = get_coord();

        if (is_inside(x, y)) {
            inside++;
        }
    }
    return inside;
}

double calc_pi(unsigned long long n, unsigned long long inside) {
    return 4.0 * inside / n;
}


void run_experiment(unsigned long long points_per_proc, unsigned long long total_points, int rank) {
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    unsigned long long points_inside = calc_inside(points_per_proc);
    unsigned long long total_inside;
    
    MPI_Reduce(&points_inside , &total_inside, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
	double pi = calc_pi(total_points, total_inside);
	double end = MPI_Wtime();
        printf("%.8f %.8f\n", pi, end - start);
    }
}

int main(int argc, char *argv[]) {

    MPI_Init(NULL, NULL);
    
    int rank, size, split_points;
    unsigned long long points_num, points_per_proc;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    points_num = strtoull(argv[1], NULL, 0);
    split_points = atoi(argv[2]);
    
    if (split_points) {
        points_per_proc = points_num / size;
    } else {
        points_per_proc = points_num;
    }

    srand(time(NULL));
    run_experiment(points_per_proc, points_per_proc * size, rank);

    MPI_Finalize();
    return 0;
}
