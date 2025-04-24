#include <iostream>
#include <mpi.h>
#include <vector>

using namespace std;

int main(int argc, char** argv) {
    int ProcNum, ProcRank; 
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum); 
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int N = 100;
    int local_sum = 0;
    int general_sum = 0;

    vector<int> local_N(ProcNum);
    vector<int> displs(ProcNum);
    int quotient = N / ProcNum;
    int remainder = N % ProcNum;

    for (int i = 0; i < ProcNum; i++) {
        local_N[i] = quotient + (i < remainder ? 1 : 0);
        displs[i] = (i == 0) ? 0 : displs[i - 1] + local_N[i - 1];
    }

    vector<int> local_array(local_N[ProcRank]);

    double start, end;

    if (ProcRank == 0) {
        vector<int> array(N, 1);
        start = MPI_Wtime();
        MPI_Scatterv(array.data(), local_N.data(), displs.data(), MPI_INT, local_array.data(), local_N[ProcRank], MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        MPI_Scatterv(nullptr, nullptr, nullptr, MPI_DATATYPE_NULL, local_array.data(), local_N[ProcRank], MPI_INT, 0, MPI_COMM_WORLD);
    }

    for (int i = 0; i < local_N[ProcRank]; i++) {
        local_sum += local_array[i];
    }

    MPI_Reduce(&local_sum, &general_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (ProcRank == 0) {
        end = MPI_Wtime();
        cout << "Общая сумма: " << general_sum << endl;
        cout << "Time " << end - start << endl;
    }

    MPI_Finalize();
    
    return 0;
}
