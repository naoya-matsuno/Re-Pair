#ifndef DEBUG_UTILS_HPP
#define DEBUG_UTILS_HPP

#include <string>
#include "external_data_handler.hpp"

template <typename T>
void debug_output(const T& data_content, const std::string& output_file_name) {
    const std::string output_file_extension = "txt";
    const std::string directory_path = "./debug_output/";
    const std::string deliimiter = ", ";
    
    OutputData output_data(output_file_name, output_file_extension, directory_path, deliimiter);
    output_data.set_data(data_content);
    output_data.output();
}

#endif