
#include <iostream>
#include <fstream>
#include <vector>

#define MAX_PROCESSES 100
#define MAX_RESOURCES 100

void readInputFile(const std::string& filename, int& numProcesses, int& numResourceTypes,
                   std::vector<int>& E, std::vector<std::vector<int>>& C, std::vector<std::vector<int>>& R) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        exit(EXIT_FAILURE);
    }

    file >> numProcesses;
    file >> numResourceTypes;

    E.resize(numResourceTypes);
    for (int i = 0; i < numResourceTypes; ++i) {
        file >> E[i];
    }

    C.resize(numProcesses, std::vector<int>(numResourceTypes));
    for (int i = 0; i < numProcesses; ++i) {
        for (int j = 0; j < numResourceTypes; ++j) {
            file >> C[i][j];
        }
    }

    R.resize(numProcesses, std::vector<int>(numResourceTypes));
    for (int i = 0; i < numProcesses; ++i) {
        for (int j = 0; j < numResourceTypes; ++j) {
            file >> R[i][j];
        }
    }

    file.close();
}

void calculateAvailableResources(int numProcesses, int numResourceTypes, const std::vector<int>& E,
                                 const std::vector<std::vector<int>>& C, std::vector<int>& A) {
    A.resize(numResourceTypes);
    for (int j = 0; j < numResourceTypes; ++j) {
        A[j] = E[j];
        for (int i = 0; i < numProcesses; ++i) {
            A[j] -= C[i][j];
        }
    }
}

bool isRequestLessThanOrEqual(int numResourceTypes, const std::vector<int>& request, const std::vector<int>& available) {
    for (int j = 0; j < numResourceTypes; ++j) {
        if (request[j] > available[j]) {
            return false;
        }
    }
    return true;
}

void addResources(int numResourceTypes, std::vector<int>& available, const std::vector<int>& allocation) {
    for (int j = 0; j < numResourceTypes; ++j) {
        available[j] += allocation[j];
    }
}

void deadlockDetection(int numProcesses, int numResourceTypes, const std::vector<std::vector<int>>& C,
                       const std::vector<std::vector<int>>& R, std::vector<int>& A) {
    std::vector<bool> finish(numProcesses, false);
    bool deadlock = false;

    for (int i = 0; i < numProcesses; ++i) {
        if (!finish[i] && isRequestLessThanOrEqual(numResourceTypes, R[i], A)) {
            addResources(numResourceTypes, A, C[i]);
            finish[i] = true;
            i = -1; // Restart the process check
        }
    }

    for (int i = 0; i < numProcesses; ++i) {
        if (!finish[i]) {
            deadlock = true;
            std::cout << "Process " << i << " is deadlocked." << std::endl;
        }
    }

    if (!deadlock) {
        std::cout << "No deadlock detected." << std::endl;
    }
}

int main() {
    int numProcesses, numResourceTypes;
    std::vector<int> E;
    std::vector<std::vector<int>> C;
    std::vector<std::vector<int>> R;
    std::vector<int> A;

    readInputFile("input.txt", numProcesses, numResourceTypes, E, C, R);
    calculateAvailableResources(numProcesses, numResourceTypes, E, C, A);
    deadlockDetection(numProcesses, numResourceTypes, C, R, A);

    return 0;
}



