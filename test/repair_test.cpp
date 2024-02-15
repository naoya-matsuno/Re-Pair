#include <iostream>
#include <string>
#include "external_data_handler.hpp"
#include "repair.hpp"

void compress_test();

int main() {
    compress_test();
}

// 圧縮のテスト
void compress_test() {
    std::string directory_path = "./test_files/canterburycorpus/";
    std::vector<std::string> test_files = get_all_file_paths_under_directory(directory_path);

    for (const std::string &test_file : test_files) {
        InputData input_data(test_file);
        const std::string text = input_data.get_all_text_by_string();

        RePair repair(text);
        repair.compress();
        if (!repair.is_equal_text_and_decompressed_text()) {
            std::cerr << "Decompression of " << input_data.get_input_file_name_with_extension() << " is failed." << std::endl;
        }

        std::cout << input_data.get_input_file_name_with_extension() << ":" << repair.get_compression_info() << std::endl;
    }
}