#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>

#include "external_data_handler.hpp"

// InputData
void InputData::set_values() {
    std::size_t path_pos = input_file_path.find_last_of("/") + 1;
    if (path_pos > input_file_path.size()) {
        path_pos = input_file_path.find_last_of("\\") + 1;
        if (path_pos > input_file_path.size())
            throw std::invalid_argument("input_file_path is not correcet.");
    }

    std::size_t extension_pos = input_file_path.find_last_of(".");
    
    input_file_name = input_file_path.substr(path_pos, extension_pos - path_pos);
    directory_path = input_file_path.substr(0, path_pos);

    if (extension_pos > input_file_path.size() - 1 || extension_pos < path_pos)
        input_file_extension = "";
    else
        input_file_extension = input_file_path.substr(extension_pos + 1, input_file_path.size() - extension_pos - 1);
}

InputData::InputData(const std::string& input_file_path) {
    this->input_file_path = input_file_path;
    set_values();
}

// 入力ファイル名を取得
std::string InputData::get_input_file_name() const {
    return input_file_name;
}

// 入力ファイル名を拡張子付きで取得
std::string InputData::get_input_file_name_with_extension() const {
    if (input_file_extension == "")
        return input_file_name;
    else
        return input_file_name + "." + input_file_extension;
}

// 入力ファイルの拡張子を取得
std::string InputData::get_input_file_extension() const {
    return input_file_extension;
}

// 入力ファイルがあるディレクトリのパスを取得
std::string InputData::get_directory_path() const {
    return directory_path;
}

// 入力ファイルのパスを取得
std::string InputData::get_input_file_path() const {
    return input_file_path;
}

// 入力ファイルのパスを変更
void InputData::set_input_file_path(const std::string& input_file_path) {
    this->input_file_path = input_file_path;
    set_values();
}

// std::string型でテキスト全てを取得
std::string InputData::get_all_text_by_string() const {
    std::string text;
    std::fstream ifs;
    
    ifs.open(input_file_path, std::ios::in);
    if (!ifs) {
        throw std::invalid_argument("Failed to load " + input_file_path + " .");
    }
    else {
        text = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
        ifs.close();
    }

    return text;
}

// std::string型でテキスト一行を取得
std::string InputData::get_line_text_by_string(const std::size_t& line_num) const {
    std::string line;
    std::fstream ifs;
    
    ifs.open(input_file_path, std::ios::in);
    if (!ifs) {
        throw std::invalid_argument("Failed to load " + input_file_path + " .");
    }
    else {
        std::size_t i = 1;
        bool is_exist = false;
        while (getline(ifs, line)) {
            if (i == line_num) {
                is_exist = true;
                break;
            }
            i++;
        }
        ifs.close();

        if (!is_exist)
            throw std::invalid_argument("The line number exceeded in this file.");
    }

    return line;
}





// OutputData
// 出力ファイルのパスを更新
void OutputData::update_output_file_path() {
    if (directory_path.at(directory_path.size() - 1) == '/')
        output_file_path = directory_path + output_file_name + "." + output_file_extension;
    else
        output_file_path = directory_path + "/" + output_file_name + "." + output_file_extension;
}

// 出力先のディレクトリがない場合に作成
void OutputData::create_directory() const {
    std::filesystem::create_directories(directory_path);   
}

// ヘッダーを書き出し
void OutputData::write_header(std::ofstream& ofs) const {
    for (std::size_t i = 0; i < header.size(); i++) {
        ofs << header[i];
        if (i < header.size() - 1)
            ofs << ",";
        else
            ofs << std::endl;
    }
}

// データを書き出し
void OutputData::write_data(std::ofstream& ofs) const {
    if (ofs) {
        for (const std::vector<std::string>& data_content : data) {
            for (std::size_t i = 0; i < data_content.size(); i++) {
                ofs << data_content[i];
                if (i < data_content.size() - 1)
                    ofs << delimiter;
                else
                    ofs << std::endl;
            }
        }
    }
}

// output_file_name: 出力ファイル名（拡張子抜き）, output_file_extension: 出力ファイルの拡張子, directory_path: 出力ファイルを保存するディレクトリのパス, delimiter: データの区切り文字
OutputData::OutputData(const std::string& output_file_name, const std::string& output_file_extension, const std::string& directory_path, const std::string& delimiter) {
    this->output_file_name = output_file_name;
    this->output_file_extension = output_file_extension;
    this->directory_path = directory_path;
    this->delimiter = delimiter;
    update_output_file_path();
}

// ヘッダーの設定
void OutputData::set_header(const std::vector<std::string>& header) {
    clear_header();
    this->header = header;
}

// ヘッダーの追加
void OutputData::append_header(const std::string& title) {
    this->header.push_back(title);
}

// ヘッダーをクリア
void OutputData::clear_header() {
    this->header.clear();
}

// 出力データの設定
void OutputData::set_data(const std::vector<std::string>& data_content) {
    clear_data();
    this->data.push_back(data_content);
}

// 出力データの設定
void OutputData::set_data(const std::vector<std::vector<std::string>>& data_contents) {
    clear_data();
    this->data = data_contents;
}

// 出力データの追加
void OutputData::append_data(const std::vector<std::string>& data_content) {
    this->data.push_back(data_content);
}

// 出力データの追加
void OutputData::append_data(const std::vector<std::vector<std::string>>& data_contents) {
    for (const std::vector<std::string>& data_content : data_contents)
        this->data.push_back(data_content);
}

// データをクリア
void OutputData::clear_data() {
    this->data.clear();
}

// output_file_nameを変更
void OutputData::set_output_file_name(const std::string& output_file_name) {
    this->output_file_name = output_file_name;
    update_output_file_path();
}

// output_file_extensionを変更
void OutputData::set_output_file_extension(const std::string& output_file_extension) {
    this->output_file_extension = output_file_extension;
    update_output_file_path();
}

// directory_pahtを変更
void OutputData::set_directory_path(const std::string& set_directory_path) {
    this->directory_path = directory_path;
    update_output_file_path();
}

// delimiter_stringを変更
void OutputData::set_delimiter_string(const std::string& delimiter) {
    this->delimiter = delimiter;
}

// std::ios::trunc 上書き, std::ios::app 末尾に追加 
void OutputData::output(const std::ios_base::openmode& mode, const bool& is_header) const {
    create_directory();

    std::ofstream ofs;
    ofs.open(output_file_path, mode);
    
    if (mode == std::ios::trunc)
        ofs << "\xEF\xBB\xBF";
    
    if (is_header)
        write_header(ofs);
    
    write_data(ofs);

    ofs.close();
}





// 複数のOutputDataオブジェクトを作成
std::vector<OutputData> create_OutputData_objects(const std::vector<std::string>& output_file_names, const std::string& output_file_extension, const std::string& output_directory_path, const std::string& delimiter) {
    std::vector<OutputData> output_data_objects;

    for (const std::string& output_file_name : output_file_names) {
        OutputData output_data_object(output_file_name, output_file_extension, output_directory_path, delimiter);
        output_data_objects.push_back(output_data_object);
    }

    return output_data_objects;
}

// ディレクトリ内のすべてのファイルパスを取得
std::vector<std::string> get_all_file_paths_in_directory(const std::string& directory_path) {
    std::vector<std::string> file_paths;
    for (const auto& file : std::filesystem::directory_iterator(directory_path)) {
        if (!file.is_directory())
            file_paths.push_back(file.path().string());
    }
    
    return file_paths;
}

// ディレクトリ下のすべてのファイルパスを取得
std::vector<std::string> get_all_file_paths_under_directory(const std::string& directory_path) {
    std::vector<std::string> file_paths;
    for (const auto& file : std::filesystem::directory_iterator(directory_path)) {
        if (!file.is_directory())
            file_paths.push_back(file.path().string());
        else
            for (const std::string& file_path : get_all_file_paths_under_directory(file.path().string()))
                file_paths.push_back(file_path);
    }
    
    return file_paths;
}

// ディレクトリ内の特定の拡張子のファイルパスを取得
std::vector<std::string> get_any_extension_file_paths_in_directory(const std::string& directory_path, const std::string& extension) {
    std::vector<std::string> file_paths;
    for (const auto& file : std::filesystem::directory_iterator(directory_path)) {
        std::string file_path = file.path().string();
        std::size_t extension_pos = file_path.find_last_of(".");

        if (!file.is_directory()) {
            if (extension_pos > file_path.size() - 1) {
                if (extension == "")
                    file_paths.push_back(file_path);
            }
            else {
                if (file_path.substr(extension_pos + 1, file_path.size() - extension_pos - 1) == extension)
                    file_paths.push_back(file_path);
            }
        }
    }
    
    return file_paths;
}

// ディレクトリ下の特定の拡張子のファイルパスを取得
std::vector<std::string> get_any_extension_file_paths_under_directory(const std::string& directory_path, const std::string& extension) {
    std::vector<std::string> file_paths;
    for (const auto& file : std::filesystem::directory_iterator(directory_path)) {
        if (!file.is_directory()) {
            std::string file_path = file.path().string();
            std::size_t extension_pos = file_path.find_last_of(".");
            
            if (extension_pos > file_path.size() - 1) {
                if (extension == "")
                    file_paths.push_back(file_path);
            }
            else {
                if (file_path.substr(extension_pos + 1, file_path.size() - extension_pos - 1) == extension)
                    file_paths.push_back(file_path);
            }
        }
        else {
            for (const std::string& file_path : get_any_extension_file_paths_under_directory(file.path().string(), extension))
                file_paths.push_back(file_path);
        }
    }
    
    return file_paths;
}