#include <iostream> 
#include <mpi.h> 
 
using namespace std; 

int main(int argc, char** argv) {     
    int ProcNum, ProcRank;     
    MPI_Init(&argc, &argv);     
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);     
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank); 
    
    int N = ProcNum / 2;     
    double A; 
    
    if (ProcRank == 0) {         
        A = 1.11;     
    } else if (ProcRank == N) {         
        A = 5.55;     
    } 
    
    int dims[2] = {2, N};     
    int periods[2] = {0, 0};     
    int reorder = 0;     
    MPI_Comm matr_Comm; 
    
    double start, end;     
    if (ProcRank == 0) start = MPI_Wtime();     
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &matr_Comm); 
    
    int subdims[2] = {0, 1};     
    MPI_Comm row_Comm;     
    MPI_Cart_sub(matr_Comm, subdims, &row_Comm); 
    
    int RowProcRank;     
    MPI_Comm_rank(row_Comm, &RowProcRank);     
    if (RowProcRank == 0) {     
        cout << "Process " << ProcRank << " is the main process in its row" << endl;     
    } 
    
    MPI_Bcast(&A, 1, MPI_DOUBLE, 0, row_Comm); 
    
    MPI_Comm_free(&row_Comm);     
    MPI_Comm_free(&matr_Comm);     
    
    if (ProcRank == 0) {             
        end = MPI_Wtime();             
        cout << "Time " << end - start << endl;     
    } 
    
    cout << "Process " << ProcRank << " has A = " << A << endl; 
    
    MPI_Finalize();     
    return 0; 
}