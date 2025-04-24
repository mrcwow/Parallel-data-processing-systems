#include <iostream>
#include <random>
#include <mpi.h>

using namespace std;

random_device rd;
mt19937 gen(rd());

int main(int argc, char** argv) {
    int ProcNum, ProcRank; 
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum); 
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int N = (ProcRank % 2 == 0) ? 0 : 1;
    
    MPI_Comm group_Comm;

    double start, end;
    if (N == 1) {
        double A;
        uniform_real_distribution<double> a_real(0.0, 100.0);
        A = a_real(gen);
        cout << "Process " << ProcRank << " with A = " << A << endl;

        if (ProcRank == 1) start = MPI_Wtime();
        MPI_Comm_split(MPI_COMM_WORLD, N, ProcRank, &group_Comm);

        double sum_A = 0.0;
        MPI_Allreduce(&A, &sum_A, 1, MPI_DOUBLE, MPI_SUM, group_Comm);

        MPI_Comm_free(&group_Comm);
        if (ProcRank == 1) {
            end = MPI_Wtime();
            cout << "Time " << end - start << endl;
        }
        
        cout << "Process " << ProcRank << " has the sum of A, which is " << sum_A << endl;
    } else {
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, ProcRank, &group_Comm);
    }

    MPI_Finalize();
    return 0;
}
