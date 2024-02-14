#ifndef REPAIR_HPP
#define REPAIR_HPP

#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include "types.hpp"

template <typename T>
class RePair {
    private:
        RePairText<T> text;
        RePairText<T> compressed_text;
        RePairText<T> decompressed_text;
        Rules<T> rules;
        RePairDataList<T> repair_data_list;

        void decompress() {
            std::list<RePairSymbol<T>> decompress_data_list;

            for (const RePairSymbol<T>& repair_symbol : compressed_text)
                decompress_data_list.push_back(repair_symbol);
            
            for (auto itr = decompress_data_list.begin(); itr != decompress_data_list.end(); itr++) {
                // 文字が非終端記号の時
                if (std::holds_alternative<NonTerminalSymbol>(*itr)) {
                    const NonTerminalSymbol non_terminal_symbol = std::get<NonTerminalSymbol>(*itr);
                    const Rule rule = rules[non_terminal_symbol.value]; // 生成規則の取得
                    // バイグラムの1文字目の処理
                    *itr = rule.first;
                    // バイグラムの2文字目の処理
                    decompress_data_list.insert(std::next(itr), rule.second);
                    // 展開後にさらに展開できる非終端記号になるかもしれないのでもう一度同じ文字を調べるためにitrを進ませない
                    itr--;
                }
            }
        }

        void init_data() {
            init_repair_data_list();
            init_search_bigrams();
            init_hash_table_and_priority_queue();
        }

        // repair_data_listの初期化
        void init_repair_data_list() {
            for (std::size_t i = 0; i < text.size(); i++) {
                const std::size_t index_num = i;
                std::size_t prev_index_num = index_num - 1;
                if (index_num == 0)
                    prev_index_num = OUT_OF_RANGE;
                std::size_t next_index_num = index_num + 1;
                if (index_num == text.size() - 1)
                    next_index_num = OUT_OF_RANGE;

                repair_data_list.push_back(RePairData(index_num, prev_index_num, next_index_num));
            }
        }

        void init_search_bigrams() {
            for (std::size_t i = 0; i < text.size() - 1; i++) {
                Bigram bigram(text[i], text[i + 1]);
            }
        }


        void init_hash_table_and_priority_queue() {

        }

        void compress_data() {

        }
    public:
        RePair() {}

        RePair(const std::vector<T>& text) {
            this->text = RePairText(text);
        }

        void compress() {
            if (text.size() >= 4) {
                init_data();
                compress_data();
            } else
                compressed_text = text;
        }

        bool is_equal_text_and_decompressed_text() {
            decompress();
            return text == decompressed_text;
        }

        Rules<T> get_rules() const {
            return rules;
        }
};

template <>
class RePair<char> {

};

#endif