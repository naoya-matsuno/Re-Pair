#ifndef REPAIR_HPP
#define REPAIR_HPP

#include <string>
#include <vector>
#include <list>
#include <utility>
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
        HashTable<T> hash_table;
        PriorityQueue priority_queue;
        ConsecutiveSymbolDataList consecutive_symbol_data_list;

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

            decompressed_text.clear();
            
            for (auto itr = decompress_data_list.begin(); itr != decompress_data_list.end(); itr++)
                decompressed_text.push_back(*itr);
        }

        void init_data() {
            rules.clear();
            init_repair_data_list();
            init_hash_table_and_priority_queue(calculate_bigram_to_bigram_record_map());
            init_consecutive_symbol_data_list();
        }

        // repair_data_listの初期化
        void init_repair_data_list() {
            repair_data_list.clear();

            for (std::size_t i = 0; i < text.size(); i++) {
                const std::size_t index_num = i;
                std::size_t prev_index_num = index_num - 1;
                if (index_num == 0)
                    prev_index_num = OUT_OF_RANGE;
                std::size_t next_index_num = index_num + 1;
                if (index_num == text.size() - 1)
                    next_index_num = OUT_OF_RANGE;

                repair_data_list.push_back(RePairData(text[index_num], index_num, prev_index_num, next_index_num));
            }
        }

        // 各バイグラムのレコードを計算
        std::unordered_map<Bigram<T>, BigramRecord> calculate_bigram_to_bigram_record_map() {
            std::unordered_map<Bigram<T>, std::vector<std::size_t>> bigram_to_positions_map;
            
            // 各バイグラムの出現位置を計算
            for (std::size_t i = 0; i < repair_data_list.size() - 1; i++) {
                Bigram<T> bigram = repair_data_list.get_bigram(i);
                if (bigram_to_positions_map.contains(bigram))
                    bigram_to_positions_map.at(bigram).push_back(i);
                else
                    bigram_to_positions_map[bigram] = std::vector<std::size_t>{i};
            }

            std::unordered_map<Bigram<T>, BigramRecord> bigram_to_bigram_record_map;

            // 各バイグラムの初出現位置，出現頻度を計算
            for (auto itr = bigram_to_positions_map.begin(); itr != bigram_to_positions_map.end(); itr++) {
                const std::size_t first_location = itr->second[0];
                std::size_t appearance_frequency = 0;

                // repair_data_listのprev_bigram_index_num, next_bigram_index_num
                for (std::size_t i = 0; i < itr->second.size(); i++) {
                    if (i > 0)
                        repair_data_list[itr->second[i]].prev_bigram_index_num = itr->second[i - 1];
                    if (i + 1 < itr->second.size())
                        repair_data_list[itr->second[i]].next_bigram_index_num = itr->second[i + 1];
                }

                // appearace_frequencyの計算
                for (std::size_t i = 0; i < itr->second.size(); i++) {
                    appearance_frequency++;
                      
                    // オーバーラップするようなバイグラムは数えない
                    if (i + 1 < itr->second.size() && itr->second[i] + 1 == itr->second[i + 1])
                        i++;
                }

                bigram_to_bigram_record_map[itr->first] = BigramRecord(first_location, appearance_frequency);
            }

            return bigram_to_bigram_record_map;
        }

        // hash_tableとpriority_queueの初期化
        void init_hash_table_and_priority_queue(const std::unordered_map<Bigram<T>, BigramRecord>& bigram_to_bigram_record_map) {
            hash_table.clear();
            priority_queue.clear();

            // 出現頻度の最大値を計算
            std::size_t max_appearance_frequency = 0;
            for (auto itr = bigram_to_bigram_record_map.begin(); itr != bigram_to_bigram_record_map.end(); itr++) {
                if (itr->second.appearance_frequency > max_appearance_frequency)
                    max_appearance_frequency = itr->second.appearance_frequency;
            }

            // hash_table, priority_queueの初期化
            priority_queue = PriorityQueue(max_appearance_frequency + 1);
            for (auto itr = bigram_to_bigram_record_map.begin(); itr != bigram_to_bigram_record_map.end(); itr++) {
                if (itr->second.appearance_frequency > 1) {
                    priority_queue[itr->second.appearance_frequency].push_back(itr->second);
                    hash_table[itr->first] = std::prev(priority_queue[itr->second.appearance_frequency].end());
                }
            }
        }

        // consecutive_symbol_data_listを初期化
        void init_consecutive_symbol_data_list() {
            consecutive_symbol_data_list = ConsecutiveSymbolDataList(std::vector<ConsecutiveSymbolData>(repair_data_list.size(), ConsecutiveSymbolData()));

            bool search_begin = true; // 連続する文字の開始位置を探索している状態
            std::size_t consecutive_count = 0;
            std::size_t begin_index_num = 0;

            for (std::size_t i = 0; i < repair_data_list.size() - 1; i++) {
                if (search_begin) {
                    if (repair_data_list.get_bigram(i).is_equal_first_and_second()) {
                        consecutive_count = 2;
                        search_begin = false;
                        begin_index_num = i;
                    }
                } else {
                    if (repair_data_list.get_bigram(i).is_equal_first_and_second()) {
                        consecutive_count++;
                        if (i == repair_data_list.size() - 2)
                            consecutive_symbol_data_list.set_consecutive_data(consecutive_count, begin_index_num, i + 1);
                    }
                    else {
                        search_begin = true;
                        consecutive_symbol_data_list.set_consecutive_data(consecutive_count, begin_index_num, i);
                    }
                }
            }
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