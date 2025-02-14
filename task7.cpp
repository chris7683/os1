#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void readInput(const string& filename, int &numProcesses, int &numResources,
               vector<int> &E, vector<vector<int>> &C, vector<vector<int>> &R) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file!" << endl;
        exit(1);
    }

    file >> numProcesses >> numResources;

    E.resize(numResources);
    for (int i = 0; i < numResources; i++)
        file >> E[i];

    C.assign(numProcesses, vector<int>(numResources));
    for (int i = 0; i < numProcesses; i++)
        for (int j = 0; j < numResources; j++)
            file >> C[i][j];

    R.assign(numProcesses, vector<int>(numResources));
    for (int i = 0; i < numProcesses; i++)
        for (int j = 0; j < numResources; j++)
            file >> R[i][j];

    file.close();
}

bool detectDeadlock(int numProcesses, int numResources, const vector<int>& E,
                    const vector<vector<int>>& C, const vector<vector<int>>& R) {
    vector<int> A(numResources, 0);
    vector<bool> finished(numProcesses, false);
    
    // Calculate Available Resources: A = E - sum(C[i])
    for (int j = 0; j < numResources; j++) {
        int sumAllocated = 0;
        for (int i = 0; i < numProcesses; i++)
            sumAllocated += C[i][j];
        A[j] = E[j] - sumAllocated;
    }

    int completedProcesses = 0;
    bool progress = true;

    while (progress) {
        progress = false;
        for (int i = 0; i < numProcesses; i++) {
            if (!finished[i]) {
                bool canExecute = true;
                for (int j = 0; j < numResources; j++) {
                    if (R[i][j] > A[j]) {
                        canExecute = false;
                        break;
                    }
                }
                if (canExecute) {
                    finished[i] = true;
                    progress = true;
                    completedProcesses++;
                    for (int j = 0; j < numResources; j++)
                        A[j] += C[i][j]; // Release resources
                }
            }
        }
    }

    // Check for deadlock
    bool deadlock = false;
    cout << "Deadlock status: ";
    for (int i = 0; i < numProcesses; i++) {
        if (!finished[i]) {
            cout << "P" << i << " ";
            deadlock = true;
        }
    }

    if (deadlock) {
        cout << "\nDeadlock detected!" << endl;
        return true;
    } else {
        cout << "No deadlock detected." << endl;
        return false;
    }
}

int main() {
    int numProcesses, numResources;
    vector<int> E;
    vector<vector<int>> C, R;

    readInput("input.txt", numProcesses, numResources, E, C, R);
    detectDeadlock(numProcesses, numResources, E, C, R);

    return 0;
}
