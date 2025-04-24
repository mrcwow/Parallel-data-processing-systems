#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char** argv){
    int ProcNum, ProcRank, RecvRank; 
    MPI_Status Status; 
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum); 
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank); 

    double start, end;
    if (ProcRank == 0) {
        // cout << "\n Hello from process " << ProcRank;
        start = MPI_Wtime();
        for (int i = 1; i < ProcNum; i++) {
            MPI_Recv(&RecvRank, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
            // cout << "\n Hello from process " << RecvRank;
        }
        end = MPI_Wtime();
        cout << "Time " << ProcNum << ": " << end - start << endl;
    } else {
        MPI_Send(&ProcRank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    
    return 0;
}