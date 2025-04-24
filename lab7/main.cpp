#include <iostream> 
#include <mpi.h> 
#include <vector> 
#include <sstream> 
 
using namespace std; 
 
void printMatr(const vector<int> &matrix, int K, int n) { 
    for (int i = 0; i < n*K; i++) { 
        if (i % K == 0 && i != 0){ 
            cout << endl; 
        } 
        cout << matrix[i] << " "; 
    } 
    cout << endl; 
} 
 
int main(int argc, char** argv) { 
    int ProcNum, ProcRank; 
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum); 
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank); 
    MPI_Status sendStatus, recvStatus; 
    MPI_Request req; 
 
    // int K = 4; 
    int K = stoi(argv[1]); 
     
    if (K % ProcNum != 0) { 
        if (ProcRank == 0) cout << "Wrong size of matrix!" << endl; 
        MPI_Finalize(); 
        return 0; 
    } 
    int n = K / ProcNum; 
 
    vector<int> local_A(n*K); 
    vector<int> local_B(n*K); 
    vector<int> local_C(n*K, 0); 
    vector<int> C; 
 
    double start, end; 
    if (ProcRank == 0) { 
        vector<int> A(K*K, 1); 
        // vector<int> A(K*K); 
        // for (int i = 0; i < K; ++i) { 
        //     string line;
        //     getline(cin, line); 
 
        //     stringstream ss(line); 
        //     for (int j = 0; j < K; ++j) { 
        //         ss >> A[i * K + j]; 
        //     } 
        // } 
        vector<int> B(K*K, 2); 
        // vector<int> B(K*K); 
        // for (int i = 0; i < K; ++i) { 
        //     string line; 
        //     getline(cin, line); 
 
        //     stringstream ss(line); 
        //     for (int j = 0; j < K; ++j) { 
        //         ss >> B[i * K + j]; 
        //     } 
        // } 
        C.resize(K * K); 
        start = MPI_Wtime(); 
        MPI_Scatter(A.data(), n * K, MPI_INT, local_A.data(), n * K, MPI_INT, 0, MPI_COMM_WORLD); 
        MPI_Scatter(B.data(), n * K, MPI_INT, local_B.data(), n * K, MPI_INT, 0, MPI_COMM_WORLD); 
    } 
    else { 
        MPI_Scatter(nullptr, n * K, MPI_DATATYPE_NULL, local_A.data(), n * K, MPI_INT, 0, MPI_COMM_WORLD); 
        MPI_Scatter(nullptr, n * K, MPI_DATATYPE_NULL, local_B.data(), n * K, MPI_INT, 0, MPI_COMM_WORLD); 
    } 
 
    int dims[1] = {ProcNum}; 
    int periods[1] = {1}; 
    int reorder = 0; 
    MPI_Comm ring_Comm; 
 
    MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, reorder, &ring_Comm); 
 
    int left, right; 
    MPI_Cart_shift(ring_Comm, 0, 1, &left, &right); 
 
    int PrevRank = ProcRank; 
 
    for (int i = 0; i < ProcNum; i++) { 
        for (int j = 0; j < n; j++){ 
            for (int q = 0; q < K; q++){ 
                for (int s = 0; s < n; s++) { 
                    local_C[q + K * j] += local_A[PrevRank * n + s + K * j] * local_B[q + K * s]; 
                } 
            } 
        } 
        PrevRank -= 1; 
        if (PrevRank < 0){ 
            PrevRank = ProcNum - 1; 
        } 
        MPI_Issend(local_B.data(), n * K, MPI_INT, right, 0, ring_Comm, &req);
        MPI_Recv(local_B.data(), n * K, MPI_INT, left, 0, ring_Comm, &recvStatus); 
        MPI_Wait(&req, &sendStatus); 
    } 
    MPI_Comm_free(&ring_Comm); 
    
    MPI_Gather(local_C.data(), n * K, MPI_INT, C.data(), n * K, MPI_INT, 0, MPI_COMM_WORLD); 
        
    if (ProcRank == 0) { 
        end = MPI_Wtime(); 
        cout << "Time " << end - start << endl; 
        printMatr(C, K, K); 
    } 
    
    MPI_Finalize(); 
    return 0; 
}