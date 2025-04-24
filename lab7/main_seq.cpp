#include <iostream> 
#include <vector> 
#include <chrono> 
#include <sstream>

using namespace std;
using namespace std::chrono; 

void printMatr(const vector<int> &matrix, int K, int n) { 
    for (int i = 0; i < n*K; i++) { 
        if (i % K == 0 && i != 0) { 
            cout << endl; 
        }
        cout << matrix[i] << " ";
    } 
    cout << endl; 
} 

vector<int> seq_mult_matr(const vector<int> &A, const vector<int> &B, int K) { 
    vector<int> C(K * K, 0); 
 
    for (int i = 0; i < K; i++) { 
        for (int j = 0; j < K; j++) { 
            for (int q = 0; q < K; q++) { 
                C[j + i * K] += A[q + i * K] * B[j + q * K]; 
            } 
        } 
    } 
 
    return C; 
}

int main(int argc, char* argv[]) { 
    // int K = 4; 
    int K = stoi(argv[1]); 
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
    vector<int> C; 
 
    auto start = steady_clock::now(); 
    C = seq_mult_matr(A, B, K); 
    auto end = steady_clock::now(); 
    duration<double> duration = end - start; 
    cout << "Time " << duration.count() << endl; 
 
    printMatr(C, K, K); 
 
    return 0; 
}