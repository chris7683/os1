#include <iostream>
#include "pages.h"  // Include the header file for function declarations

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <num_frames>\n";
        return 1;
    }

    std::string file_name = argv[1];
    int frame_count = std::atoi(argv[2]);

    if (frame_count <= 0) {
        std::cerr << "Error: Number of frames must be greater than zero.\n";
        return 1;
    }

    // Create an instance of AgingAlgorithm
    AgingAlgorithm agingSim(frame_count);

    // Execute the paging algorithm using the input file
    agingSim.executeFromFile(file_name);

    return 0;
}
