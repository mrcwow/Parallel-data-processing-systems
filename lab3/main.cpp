#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <mpi.h>

using namespace std;

std::random_device rd;
std::mt19937 gen(rd());

void generateRandomRoute(vector<int>& route, int ProcNum) {
    for (int i = 1; i < ProcNum; i++) {
        route.push_back(i);
    }
    std::shuffle(route.begin(), route.end(), gen);
}

void generateRandomInfoPart(string& infoPart, int infoPartLen) {
    for (int i = 0; i < infoPartLen; i++) {
        char letter = 'a' + (gen() % 26);
        infoPart += letter;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int ProcNum, ProcRank;
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    double start, end;

    if (ProcRank == 0) {
        vector<int> route;
        generateRandomRoute(route, ProcNum);
        string routePart;
        for (int r : route) {
            routePart += to_string(r) + "|";
        }
        // string infoPart = "Hello, this is the part with information.";
        string infoPart;
        generateRandomInfoPart(infoPart, 100);
        string fullMessage = routePart + infoPart;

        // cout << "Process 0 and message '" << fullMessage << "'" << endl;

        start = MPI_Wtime();
        MPI_Send(fullMessage.c_str(), fullMessage.size() + 1, MPI_CHAR, route[0], 0, MPI_COMM_WORLD);
        // cout << "Process " << ProcRank << " sent message to Process " << route[0] << endl;
    } else {
        char msg[200];
        MPI_Recv(msg, sizeof(msg), MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // cout << "Process " << ProcRank << " received message: '" << msg << "'" << endl;

        string message(msg);
        message.erase(message.begin(), message.begin() + message.find('|') + 1);
        int pos = message.find('|'); 

        if (pos != string::npos) {
            int nextRoute = stoi(message.substr(0, pos));

            MPI_Send(message.c_str(), message.size() + 1, MPI_CHAR, nextRoute, 0, MPI_COMM_WORLD);
            // cout << "Process " << ProcRank << " forwarded message to Process " << nextRoute << endl;
        } else {
            // cout << "Process " << ProcRank << " was final and received the message: " << message << endl;
            MPI_Send(&ProcRank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            // cout << "Process " << ProcRank << " reported to Process 0." << endl;
        }
    }

    if (ProcRank == 0) {
        int FinalRank;
        MPI_Recv(&FinalRank, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        end = MPI_Wtime();
        // cout << "Final addressee, Process " << FinalRank << ", received the message and reported to me, Process 0." << endl;
        cout << "Time " << end - start << endl;
    }

    MPI_Finalize();
    return 0;
}
