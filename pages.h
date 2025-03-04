







#ifndef PAGES_H
#define PAGES_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

#define BIT_SIZE 8  // Number of bits for aging counter

// Structure for page frame
struct Frame {
    int frame_id;           // Page ID
    unsigned char age_counter;  // Aging counter
    Frame();  // Constructor
};

// Class for the Aging Algorithm
class AgingAlgorithm {
private:
    std::vector<Frame> frame_list;  // List of frames
    int frame_count;   // Number of frames
    int fault_count;   // Number of page faults
    int total_references;  // Total number of page references

public:
    AgingAlgorithm(int num_frames);  // Constructor
    int selectFrameToReplace();  // Selects the least frequently used frame
    void processPageReference(int reference_id);  // Processes a page reference
    void executeFromFile(const std::string &filename);  // Runs the algorithm using an input file
};
