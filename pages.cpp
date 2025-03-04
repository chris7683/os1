#endif

#include "pages.h"

// Constructor for the Frame structure
Frame::Frame() : frame_id(-1), age_counter(0) {}

// Constructor for the Aging Algorithm class
AgingAlgorithm::AgingAlgorithm(int num_frames) 
    : frame_count(num_frames), fault_count(0), total_references(0) {
    frame_list = std::vector<Frame>(frame_count);
}

// Selects the frame with the smallest age counter for replacement
int AgingAlgorithm::selectFrameToReplace() {
    int min_idx = 0;
    unsigned char min_value = frame_list[0].age_counter;

    for (int i = 1; i < frame_count; i++) {
        if (frame_list[i].age_counter < min_value) {
            min_value = frame_list[i].age_counter;
            min_idx = i;
        }
    }
    return min_idx;
}

// Processes a single page reference
void AgingAlgorithm::processPageReference(int reference_id) {
    total_references++;
    bool exists = false;

    // Check if the page is already in memory
    for (auto &frame : frame_list) {
        if (frame.frame_id == reference_id) {
            exists = true;
            frame.age_counter |= (1 << (BIT_SIZE - 1));  // Set the MSB
            break;
        }
    }

    // If page fault occurs, replace the least frequently used page
    if (!exists) {
        fault_count++;
        int replace_idx = selectFrameToReplace();
        frame_list[replace_idx].frame_id = reference_id;
        frame_list[replace_idx].age_counter = (1 << (BIT_SIZE - 1));  // Reset age counter
    }

    // Right shift age counters of all frames
    for (auto &frame : frame_list) {
        frame.age_counter >>= 1;
    }
}

// Reads page references from a file and processes them
void AgingAlgorithm::executeFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening input file!\n";
        exit(1);
    }

    int reference_id;
    while (file >> reference_id) {
        processPageReference(reference_id);
    }

    file.close();

    // Print results
    std::cout << "Total references: " << total_references << std::endl;
    std::cout << "Page faults: " << fault_count << std::endl;
    if (total_references > 0) {
        std::cout << "Page faults per 1000 references: " 
                  << (static_cast<float>(fault_count) / total_references) * 1000 
                  << std::endl;
    }
}
