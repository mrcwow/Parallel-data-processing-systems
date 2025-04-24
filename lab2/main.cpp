#include <iostream>
#include <mpi.h>
#include <vector>

using namespace std;

int main(int argc, char** argv) {
    int ProcNum, ProcRank;
    MPI_Status Status;  
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum); 
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    int N = 100000;
    vector<int> array(N);
    
    int local_sum = 0;
    int general_sum = 0;

    if (ProcRank == 0) {
        for (size_t i = 0; i < N; i++) {
            array[i] = 1;
        }
    }

    int quotient = N / (ProcNum - 1);
    int remainder = N % (ProcNum - 1);
    int local_N = quotient + (ProcRank - 1 < remainder ? 1 : 0);

    vector<int> local_array(local_N);

    double start, end;

    if (ProcRank == 0) {
        int offset = 0;
        start = MPI_Wtime();
        for (int i = 1; i < ProcNum; i++) {
            int send_count = quotient + (i - 1 < remainder ? 1 : 0);
            
            MPI_Send(&array[offset], send_count, MPI_INT, i, 0, MPI_COMM_WORLD);
            offset += send_count;
        }
    } else {
        MPI_Recv(local_array.data(), local_N, MPI_INT, 0, 0, MPI_COMM_WORLD, &Status);
    }

    if (ProcRank != 0) {
        for (size_t i = 0; i < local_N; i++) {
            local_sum += local_array[i];
        }

        MPI_Send(&local_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        general_sum = 0;
        for (int i = 1; i < ProcNum; i++) {
            int received_sum;
            MPI_Recv(&received_sum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &Status);
            general_sum += received_sum;
        }
        end = MPI_Wtime();
    }

    if (ProcRank == 0) {
        cout << "Общая сумма: " << general_sum << endl;
        cout << "Time " << end - start << endl;
    }

    MPI_Finalize();
    
    return 0;
}
