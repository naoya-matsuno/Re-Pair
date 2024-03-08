#ifndef EXTERNAL_DATA_HANDLER_HPP
#define EXTERNAL_DATA_HANDLER_HPP

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>

// データの入力（とりあえず文字列で読み込むものだけを実装）
class InputData {
    private:
        std::string input_file_name; // 入力ファイル名
        std::string input_file_extension; // 入力ファイルの拡張子
        std::string directory_path; // 入力ファイルがあるディレクトリのパス
        std::string input_file_path; // 入力ファイルのパス

        void set_values();
    
    public:
        InputData(const std::string& input_file_path);

        // 入力ファイル名を取得
        std::string get_input_file_name() const;
        
        // 入力ファイル名を拡張子付きで取得
        std::string get_input_file_name_with_extension() const;

        // 入力ファイルの拡張子を取得
        std::string get_input_file_extension() const;

        // 入力ファイルがあるディレクトリのパスを取得
        std::string get_directory_path() const;

        // 入力ファイルのパスを取得
        std::string get_input_file_path() const;

        // 入力ファイルのパスを変更
        void set_input_file_path(const std::string& input_file_path);

        // std::string型でテキスト全てを取得
        std::string get_all_text_by_string() const;

        // std::string型でテキスト一行を取得
        std::string get_line_text_by_string(const std::size_t& line_num) const;
};

// データの出力
class OutputData {
    private:
        std::string output_file_name; // 出力ファイル名
        std::string output_file_extension; // 出力ファイル拡張子
        std::string directory_path; // 出力ファイルを保存するディレクトリのパス
        std::string output_file_path; // 出力ファイルのパス
        std::string delimiter; // データの区切りの設定
        std::vector<std::string> header; // ヘッダー
        std::vector<std::vector<std::string>> data; // 出力データ

        // 出力ファイルのパスを更新
        void update_output_file_path();
        
        // 出力先のディレクトリがない場合に作成
        void create_directory() const;

        // ヘッダーを書き出し
        void write_header(std::ofstream& ofs) const;

        // データを書き出し
        void write_data(std::ofstream& ofs) const;

        // データをstd::vector<std::string>に変換して返す
        template <typename T, typename ...Args>
        std::vector<std::string> format_data(const T& data_content, const Args& ...data_contents) {
            std::vector<std::string> data;
            std::ostringstream oss;

            oss << data_content;
            data.push_back(oss.str());
            
            std::vector<std::string> rest_data = format_data(data_contents...);
            data.insert(data.end(), std::make_move_iterator(rest_data.begin()), std::make_move_iterator(rest_data.end()));

            return data;
        }

        template <typename T, typename ...Args>
        std::vector<std::string> format_data(const std::vector<T>& data_content, const Args& ...data_contents) {
            std::vector<std::string> data;
            std::ostringstream oss;

            oss << "[";
            for (auto itr = data_content.begin(); itr != data_content.end(); itr++) {
                oss << data_content[i];
                if (std::next(itr) != data_content.end() - 1)
                    oss << ", ";
            }
            oss << "]";
            
            data.push_back(oss.str());
            
            std::vector<std::string> rest_data = format_data(data_contents...);
            data.insert(data.end(), std::make_move_iterator(rest_data.begin()), std::make_move_iterator(rest_data.end()));

            return data;
        }

        // データをstd::vector<std::string>に変換して返す
        template <typename T>
        std::vector<std::string> format_data(const T& data_content) {
            std::vector<std::string> data;
            std::ostringstream oss;

            oss << data_content;
            data.push_back(oss.str());

            return data;
        }

        template <typename T>
        std::vector<std::string> format_data(const std::vector<T>& data_content) {
            std::vector<std::string> data;
            std::ostringstream oss;

            oss << "[";
            for (auto itr = data_content.begin(); itr != data_content.end(); itr++) {
                oss << data_content[i];
                if (std::next(itr) != data_content.end() - 1)
                    oss << ", ";
            }
            oss << "]";
            
            data.push_back(oss.str());

            return data;
        }

    public:
        // output_file_name: 出力ファイル名（拡張子抜き）, output_file_extension: 出力ファイルの拡張子, directory_path: 出力ファイルを保存するディレクトリのパス, delimiter: データの区切り文字
        OutputData(const std::string& output_file_name, const std::string& output_file_extension, const std::string& directory_path, const std::string& delimiter);

        // ヘッダーの設定
        void set_header(const std::vector<std::string>& header);

        // ヘッダーの設定
        template <typename ...Args>
        void set_header(const Args& ...titles) {
            this->header.clear();
            this->header = format_data(titles...);
        }

        // ヘッダーの追加
        void append_header(const std::string& title);

        // ヘッダーをクリア
        void clear_header();

        // 出力データの設定
        void set_data(const std::vector<std::string>& data_content);
        
        // 出力データの設定
        void set_data(const std::vector<std::vector<std::string>>& data_contents);
        
        // 出力データの設定
        template <typename ...Args>
        void set_data(const Args& ...data_contents) {
            this->data.clear();
            this->data.push_back(format_data(data_contents...));
        }

        // 出力データの追加
        void append_data(const std::vector<std::string>& data_content);

        // 出力データの追加
        void append_data(const std::vector<std::vector<std::string>>& data_contents);

        // 出力データの追加
        template <typename ...Args>
        void append_data(const Args& ...data_contents) {
            this->data.push_back(format_data(data_contents...));
        }

        // データをクリア
        void clear_data();

        // output_file_nameを変更
        void set_output_file_name(const std::string& output_file_name);
        
        // output_file_extensionを変更
        void set_output_file_extension(const std::string& output_file_extension);

        // directory_pahtを変更
        void set_directory_path(const std::string& set_directory_path);

        // delimiter_stringを変更
        void set_delimiter_string(const std::string& delimiter);

        // std::ios::trunc 上書き, std::ios::app 末尾に追加 
        void output(const std::ios_base::openmode& mode=std::ios::trunc, const bool& is_header=false) const;
};

// 複数のOutputDataオブジェクトを作成
std::vector<OutputData> create_OutputData_objects(const std::vector<std::string>& output_file_names, const std::string& output_file_extension, const std::string& output_directory_path, const std::string& delimiter);

// ディレクトリ内のすべてのファイルパスを取得
std::vector<std::string> get_all_file_paths_in_directory(const std::string& directory_path);

// ディレクトリ下のすべてのファイルパスを取得
std::vector<std::string> get_all_file_paths_under_directory(const std::string& directory_path);

// ディレクトリ内の特定の拡張子のファイルパスを取得
std::vector<std::string> get_any_extension_file_paths_in_directory(const std::string& directory_path, const std::string& extension);

// ディレクトリ下の特定の拡張子のファイルパスを取得
std::vector<std::string> get_any_extension_file_paths_under_directory(const std::string& directory_path, const std::string& extension);

#endif