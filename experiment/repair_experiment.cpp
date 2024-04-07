#include <iostream>
#include <string>
#include "external_data_handler.hpp"
#include "repair.hpp"


int main() {
    std::string file_path = ""; // 圧縮したいファイルのパスを指定
    InputData input_data(file_path);

    const std::string text = input_data.get_all_text_by_string();

    RePair repair(text);
    repair.compress();

    std::cout << input_data.get_input_file_name_with_extension() << ": " << repair.get_compression_info() << std::endl;
}