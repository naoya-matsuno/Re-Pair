#ifndef REPAIR_HPP
#define REPAIR_HPP

#include <string>
#include <vector>
#include <list>
#include <utility>
#include <unordered_map>
#include "types.hpp"

template <typename T = char>
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

        // 展開
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
            
            for (const RePairSymbol<T>& repair_symbol : decompress_data_list)
                decompressed_text.push_back(repair_symbol);
        }

        // データの初期化
        void init_data() {
            rules.clear();
            init_repair_data_list();
            init_hash_table_and_priority_queue(init_calculate_bigram_to_bigram_record_map());
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

        // 各バイグラムのレコードを計算（前処理時）
        std::unordered_map<Bigram<T>, BigramRecord> init_calculate_bigram_to_bigram_record_map() {
            std::unordered_map<Bigram<T>, std::vector<std::size_t>> bigram_to_positions_map;
            
            // 各バイグラムの出現位置を計算
            for (std::size_t i = 0; i < repair_data_list.size() - 1; i++) {
                Bigram<T> bigram = repair_data_list.get_bigram(i);
                if (bigram_to_positions_map.contains(bigram))
                    bigram_to_positions_map.at(bigram).push_back(i);
                else
                    bigram_to_positions_map[bigram] = std::vector<std::size_t>{i};
            }

            return calculate_bigram_to_bigram_record_map(bigram_to_positions_map);
        }

        // 各バイグラムのレコードを計算
        std::unordered_map<Bigram<T>, BigramRecord> calculate_bigram_to_bigram_record_map(const std::unordered_map<Bigram<T>, std::vector<std::size_t>>& bigram_to_positions_map) {
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

                if (appearance_frequency > 1)
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
            update_hash_table_and_priority_queue(bigram_to_bigram_record_map);
        }

        // hash_tableとpriority_queueの更新
        void update_hash_table_and_priority_queue(const std::unordered_map<Bigram<T>, BigramRecord>& bigram_to_bigram_record_map) {
            for (auto itr = bigram_to_bigram_record_map.begin(); itr != bigram_to_bigram_record_map.end(); itr++) {
                if (itr->second.appearance_frequency > 1) {
                    priority_queue[itr->second.appearance_frequency].push_back(itr->second);
                    hash_table[itr->first] = std::prev(priority_queue[itr->second.appearance_frequency].end());
                }
            }
        }

        // hash_tableとpriority_queueの更新
        void update_hash_table_and_priority_queue(const Bigram<T>& bigram, const std::size_t& index_num) {
            const std::size_t appearance_frequency = hash_table.at(bigram)->appearance_frequency - 1;
            std::size_t first_location = hash_table.at(bigram)->first_location;
            if (index_num == first_location)
                first_location = repair_data_list[first_location].next_bigram_index_num;
            
            priority_queue[appearance_frequency + 1].erase(hash_table.at(bigram));

            if (appearance_frequency > 2) {
                BigramRecord bigram_record(first_location, appearance_frequency);
                priority_queue[appearance_frequency].push_back(bigram_record);
                hash_table[bigram] = std::prev(priority_queue[appearance_frequency].end());
            } else
                hash_table.erase(bigram);

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

        // consecutive_symbol_data_listの更新
        void update_consecutive_symbol_data_list(const std::unordered_map<Bigram<T>, std::vector<std::size_t>>& new_bigram_to_positions_map) {
            for (auto itr = new_bigram_to_positions_map.begin(); itr != new_bigram_to_positions_map.end(); itr++) {
                if (itr->first.is_equal_first_and_second()) {
                    bool search_end = false;
                    std::size_t begin_index_num = 0;
                    std::size_t consecutive_count = 0;
                    
                    for (std::size_t i = 0; i < itr->second.size(); i++) {
                        if (!search_end) {
                            search_end = true;
                            begin_index_num = itr->second[i];
                            consecutive_count = 2;
                        } else {
                            if (repair_data_list[itr->second[i - 1]].next_index_num == itr->second[i])
                                consecutive_count++;    
                            else {
                                search_end = false;
                                consecutive_symbol_data_list.set_consecutive_data(consecutive_count, begin_index_num, repair_data_list[begin_index_num].next_index_num);
                                i--;
                            }
                        }

                        if (i == itr->second.size() - 1) {
                            search_end = false;
                            consecutive_symbol_data_list.set_consecutive_data(consecutive_count, begin_index_num, repair_data_list[begin_index_num].next_index_num);
                        }
                    }
                }
            }
        }

        // 圧縮処理
        void compress_data() {
            std::size_t max_appearance_frequency = priority_queue.size() - 1;

            while (true) {
                if (max_appearance_frequency < 2)
                    break;
                
                // リストの先頭からレコードを取り出す
                BigramRecord bigram_record(priority_queue[max_appearance_frequency].front());
                priority_queue[max_appearance_frequency].pop_front();

                // 生成規則の追加
                Bigram bigram = repair_data_list.get_bigram(bigram_record.first_location);
                Rule rule(bigram, max_appearance_frequency, rules.size());
                const NonTerminalSymbol nonterminal_symbol(rules.size());
                rules.push_back(rule);

                std::size_t index_num = bigram_record.first_location;
                std::unordered_map<Bigram<T>, std::vector<std::size_t>> new_bigram_to_positions_map; // 置き換えにより新たにできたバイグラムの出現位置

                while (index_num != OUT_OF_RANGE) {
                    // 置き換えるバイグラムがaaのように同じ文字が連続するものの時，consecutive_symbol_data_listを更新
                    if (consecutive_symbol_data_list[index_num].is_begin)
                        consecutive_symbol_data_list.delete_consecutive_symbol(index_num);
                    
                    // 左右のバイグラムの出現頻度の更新
                    update_adjacent_bigrams(index_num, new_bigram_to_positions_map);
                    
                    // バイグラムを非終端記号にする
                    repair_data_list.replace_with_nonterminal_symbol(index_num, nonterminal_symbol);

                    // 新たなバイグラムの追加
                    std::size_t left_index_num = repair_data_list[index_num].prev_index_num;
                    std::size_t right_index_num = index_num;
                    if (left_index_num != OUT_OF_RANGE)
                        new_bigram_to_positions_map[repair_data_list.get_bigram(left_index_num)].push_back(left_index_num);
                    if (repair_data_list[right_index_num].next_index_num != OUT_OF_RANGE)
                        new_bigram_to_positions_map[repair_data_list.get_bigram(right_index_num)].push_back(right_index_num);

                    // index_numの更新
                    index_num = repair_data_list[index_num].next_bigram_index_num;
                }

                std::unordered_map<Bigram<T>, BigramRecord> new_bigram_to_bigram_record_map = calculate_bigram_to_bigram_record_map(new_bigram_to_positions_map);

                update_hash_table_and_priority_queue(new_bigram_to_bigram_record_map);

                update_consecutive_symbol_data_list(new_bigram_to_positions_map);

                // max_appearance_frequencyの更新
                for (; max_appearance_frequency > 1; max_appearance_frequency--) {
                    if (priority_queue[max_appearance_frequency].size() > 0)
                        break;
                }
            }

            // compressed_textに圧縮結果を代入
            compressed_text.clear();
            std::size_t index_num = 0;
            while (true) {
                compressed_text.push_back(repair_data_list[index_num].repair_symbol);
                index_num = repair_data_list[index_num].next_index_num;
                if (index_num == OUT_OF_RANGE)
                    break;
            }
        }

        // 左右のバイグラムの出現頻度の更新
        void update_adjacent_bigrams(std::size_t& index_num, std::unordered_map<Bigram<T>, std::vector<std::size_t>>& new_bigram_to_positions_map) {
            const Bigram bigram = repair_data_list.get_bigram(index_num);
            const std::size_t left_index_num = repair_data_list[index_num].prev_index_num;
            const std::size_t right_index_num = repair_data_list[index_num].next_index_num;

            // 左側のバイグラムの処理
            if (left_index_num != OUT_OF_RANGE) {
                Bigram left_bigram = repair_data_list.get_bigram(left_index_num);

                if (hash_table.contains(left_bigram)) {
                    if (left_bigram.is_equal_first_and_second()) {
                        if (consecutive_symbol_data_list[index_num].consecutive_count % 2 == 0)
                            update_hash_table_and_priority_queue(left_bigram, left_index_num);
                        consecutive_symbol_data_list.update_consecutive_symbol(index_num, left_index_num);
                    } else
                        update_hash_table_and_priority_queue(left_bigram, left_index_num);
                } else if (new_bigram_to_positions_map.contains(left_bigram)) {
                    new_bigram_to_positions_map.at(left_bigram).pop_back();
                    if (new_bigram_to_positions_map.at(left_bigram).size() == 0)
                        new_bigram_to_positions_map.erase(left_bigram);
                }
            }

            // 右側のバイグラムの出現頻度の更新
            if (repair_data_list[right_index_num].next_index_num != OUT_OF_RANGE) {
                Bigram right_bigram = repair_data_list.get_bigram(right_index_num);

                if (bigram != right_bigram && hash_table.contains(right_bigram)) {
                    if (right_bigram.is_equal_first_and_second()) {
                        if (consecutive_symbol_data_list[right_index_num].consecutive_count % 2 == 0)
                            update_hash_table_and_priority_queue(right_bigram, right_index_num);
                        consecutive_symbol_data_list.update_consecutive_symbol(right_index_num, repair_data_list[right_index_num].next_index_num);
                    } else
                        update_hash_table_and_priority_queue(right_bigram, right_index_num);
                }
            }
        }

    public:
        RePair() {}

        RePair(const std::vector<T>& text) {
            this->text = RePairText(text);
        }

        template <typename U = T>
        RePair(const std::string& text, typename std::enable_if<std::is_same<U, char>::value>::type* = nullptr) {
            this->text.clear();
            for (const char& value : text)
                this->text.push_back(RePairSymbol(TerminalSymbol(value)));
        }

        // 圧縮
        void compress() {
            if (text.size() >= 4) {
                init_data();
                compress_data();
            } else
                compressed_text = text;
        }

        // 展開したテキストが圧縮前のものと等しいか
        bool is_equal_text_and_decompressed_text() {
            decompress();
            return text == decompressed_text;
        }

        // 生成規則の取得
        Rules<T> get_rules() const {
            return rules;
        }
};

#endif