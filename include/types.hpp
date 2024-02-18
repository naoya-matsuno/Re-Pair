#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <variant>
#include <string>
#include <utility>
#include <vector>
#include <list>
#include <limits>
#include <unordered_map>

const std::size_t OUT_OF_RANGE = std::numeric_limits<std::size_t>::max(); // nullポインタ的なもの

// 終端記号
template <typename T>
struct TerminalSymbol {
    T value;

    TerminalSymbol() {}

    TerminalSymbol(const T& value) : value(value) {}

    TerminalSymbol(const TerminalSymbol<T>& terminal_symbol) : value(terminal_symbol.value) {}

    std::string to_string() const {
        return std::to_string(value);
    }

    bool operator==(const TerminalSymbol<T>& rhs) const {
        return this->value == rhs.value;
    }

    bool operator<(const TerminalSymbol<T>& rhs) const {
        return this->value < rhs.value;
    }

    bool operator>(const TerminalSymbol<T>& rhs) const {
        return this->value > rhs.value;
    }
};

// std::hashの特殊化（TerminalSymbol）
template <typename T>
struct std::hash<TerminalSymbol<T>> {
    std::size_t operator()(const TerminalSymbol<T>& terminal_symbol) const {
        return std::hash<T>()(terminal_symbol.value);
    }
};

// 非終端記号
struct NonTerminalSymbol {
    std::size_t value;

    NonTerminalSymbol();

    NonTerminalSymbol(const std::size_t& value);

    NonTerminalSymbol(const NonTerminalSymbol& nonterminal_symbol);

    std::string to_string() const;

    bool operator==(const NonTerminalSymbol& rhs) const;

    bool operator<(const NonTerminalSymbol& rhs) const;

    bool operator>(const NonTerminalSymbol& rhs) const;
};

// std::hashの特殊化（NonTerminalSymbol）
template <>
struct std::hash<NonTerminalSymbol> {
    std::size_t operator()(const NonTerminalSymbol& nonterminal_symbol) const {
        return std::hash<std::size_t>()(nonterminal_symbol.value);
    }
};

// テキスト内に現れるシンボル
template <typename T>
struct RePairSymbol : std::variant<TerminalSymbol<T>, NonTerminalSymbol> {
    RePairSymbol() {}

    RePairSymbol(const TerminalSymbol<T>& terminal_symbol) : std::variant<TerminalSymbol<T>, NonTerminalSymbol>(terminal_symbol) {}

    RePairSymbol(const NonTerminalSymbol& nonterminal_symbol) : std::variant<TerminalSymbol<T>, NonTerminalSymbol>(nonterminal_symbol) {}

    RePairSymbol(const RePairSymbol<T>& repair_symbol) : std::variant<TerminalSymbol<T>, NonTerminalSymbol>(repair_symbol) {}

    std::string to_string() const {
        if (std::holds_alternative<TerminalSymbol<T>>(*this))
            return std::get<TerminalSymbol<T>>(*this).to_string();
        else
            return std::get<NonTerminalSymbol>(*this).to_string();
    }

    bool operator<(const RePairSymbol& rhs) const {
        if (std::holds_alternative<TerminalSymbol<T>>(*this)) {
            if (std::holds_alternative<TerminalSymbol<T>>(rhs))
                return std::get<TerminalSymbol<T>>(*this) < std::get<TerminalSymbol<T>>(rhs);
            else
                return false;
        } else {
            if (std::holds_alternative<NonTerminalSymbol>(rhs))
                return std::get<NonTerminalSymbol>(*this) < std::get<NonTerminalSymbol>(rhs);
            else
                return true;
        }
    }

    bool operator>(const RePairSymbol& rhs) const {
        if (std::holds_alternative<TerminalSymbol<T>>(*this)) {
            if (std::holds_alternative<TerminalSymbol<T>>(rhs))
                return std::get<TerminalSymbol<T>>(*this) > std::get<TerminalSymbol<T>>(rhs);
            else
                return true;
        } else {
            if (std::holds_alternative<NonTerminalSymbol>(rhs))
                return std::get<NonTerminalSymbol>(*this) > std::get<NonTerminalSymbol>(rhs);
            else
                return false;
        }
    }
};

// std::hashの特殊化（RePairSymbol）
template <typename T>
struct std::hash<RePairSymbol<T>> {
    std::size_t operator()(const RePairSymbol<T>& repair_symbol) const {
        if (std::holds_alternative<TerminalSymbol<T>>(repair_symbol))
            return std::hash<TerminalSymbol<T>>()(std::get<TerminalSymbol<T>>(repair_symbol));
        else
            return std::hash<NonTerminalSymbol>()(std::get<NonTerminalSymbol>(repair_symbol));
    }
};

// RePairSymbolのvector
template <typename T = char>
struct RePairText : std::vector<RePairSymbol<T>> {
    RePairText() {}

    RePairText(const RePairSymbol<T>& repair_symbol) : std::vector<RePairSymbol<T>>{repair_symbol} {}

    RePairText(const RePairText<T>& repair_text) : std::vector<RePairSymbol<T>>(repair_text) {}
    
    RePairText(const std::initializer_list<RePairSymbol<T>>& repair_symbol_list) : std::vector<RePairSymbol<T>>(repair_symbol_list) {}

    RePairText(const std::initializer_list<T>& T_list) {
        for (const T& value : T_list)
            this->push_back(RePairSymbol(TerminalSymbol(value)));
    }

    template <typename U = T>
    RePairText(const std::string& text, typename std::enable_if<std::is_same<U, char>::value>::type* = nullptr) {
        for (const char& value : text)
            this->push_back(RePairSymbol(TerminalSymbol(value)));
    }

    std::string to_string() const {
        std::string str = "";
        
        for (const RePairSymbol<T>& symbol : *this)
            str += symbol.to_string();
        
        return str;
    }
};

// バイグラム
template <typename T>
struct Bigram : std::pair<RePairSymbol<T>, RePairSymbol<T>> {
    Bigram() {}

    Bigram(const RePairSymbol<T>& first, const RePairSymbol<T>& second) : std::pair<RePairSymbol<T>, RePairSymbol<T>>(first, second) {}

    Bigram(const Bigram<T>& bigram) : std::pair<RePairSymbol<T>, RePairSymbol<T>>(bigram) {}

    std::string to_string() const {
        return this->first.to_string() + this->second.to_string();
    }

    // バイグラムの1文字目と2文字目が等しいか
    bool is_equal_first_and_second() const {
        return this->first == this->second;
    }

    bool operator==(const Bigram& rhs) const {
        return this->first == rhs.first && this->second == rhs.second;
    }

    bool operator<(const Bigram& rhs) const {
        if (this->first < rhs.first)
            return true;
        else if (this->first == rhs.first)
            return this->second < rhs.second;
        else
            return false;
    }

    bool operator>(const Bigram& rhs) const {
        if (this->first > rhs.first)
            return true;
        else if (this->first == rhs.first)
            return this->second > rhs.second;
        else
            return false;
    }
};

// std::hashの特殊化（Bigram）
template <typename T>
struct std::hash<Bigram<T>> {
    std::size_t operator()(const Bigram<T>& bigram) const {
        std::size_t seed = 0;

        seed ^= std::hash<RePairSymbol<T>>()(bigram.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<RePairSymbol<T>>()(bigram.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        
        return seed;
    }
};

// Bigramのvector
template <typename T>
struct Bigrams : std::vector<Bigram<T>> {
    Bigrams() {}

    Bigrams(const Bigram<T>& bigram) : std::vector<Bigram<T>>({bigram}) {}

    Bigrams(const Bigrams<T>& bigrams) : std::vector<Bigram<T>>(bigrams) {}

    Bigrams(const std::initializer_list<Bigram<T>>& initializer_list) : std::vector<Bigram<T>>(initializer_list) {}

    std::string to_string() const {
        std::string str = "[";
        
        for (const Bigram<T>& bigram : *this) {
            str += bigram.to_string() + ", ";
        }

        if (this->size() > 0)
            str.erase(str.size() - 2, 2);
        
        str += "]";

        return str;
    }
};

// 生成規則
template <typename T>
struct Rule : std::pair<RePairSymbol<T>, RePairSymbol<T>> {
    std::size_t appearance_frequency; // 生成規則右辺のバイグラムの出現頻度
    std::size_t generated_number; // 何個目に生成された規則か(0スタート)

    Rule() {}

    Rule(const RePairSymbol<T>& first, const RePairSymbol<T>& second, const std::size_t& appearance_frequency, const std::size_t& generated_number)
    : std::pair<RePairSymbol<T>, RePairSymbol<T>>(first, second) {}

    Rule(const Rule<T>& rule) : std::pair<RePairSymbol<T>, RePairSymbol<T>>(rule), appearance_frequency(rule.appearance_frequency), generated_number(rule.generated_number) {}

    Rule(const Bigram<T>& bigram, const std::size_t& appearance_frequency, const std::size_t& generated_number)
    : std::pair<RePairSymbol<T>, RePairSymbol<T>>(bigram.first, bigram.second), appearance_frequency(appearance_frequency), generated_number(generated_number) {}

    std::string to_string() const {
        return std::to_string(generated_number) + "->" + this->first.to_string() + this->second.to_string();
    }
};

// Rulesのvector
template <typename T>
struct Rules : std::vector<Rule<T>> {
    Rules() {}

    Rules(const Rule<T>& rule) : std::vector<Rule<T>>{rule} {}

    Rules(const Rules<T>& rules) : std::vector<Rule<T>>(rules) {}

    Rules(const std::initializer_list<Rule<T>>& initializer_list) : std::vector<Rule<T>>(initializer_list) {}

    std::string to_string() const {
        std::string str = "[";
        
        for (const Rule<T>& rule : *this) {
            str += rule.to_string() + ", ";
        }
        
        if (this->size() > 0)
            str.erase(str.size() - 2, 2);
        str += "]";

        return str;
    }
};

// Re-Pairを行う際に使う構造体
template <typename T>
struct RePairData {
    RePairSymbol<T> repair_symbol;
    std::size_t index_num; // RepairDataListにおけるインデックス番号
    std::size_t prev_index_num; // RepairDataListにおける直前のデータのインデックス番号
    std::size_t next_index_num; // RepairDataListにおける直後のデータのインデックス番号
    std::size_t prev_bigram_index_num; // 直前の同じバイグラムのインデックス番号
    std::size_t next_bigram_index_num; // 直後の同じバイグラムのインデックス番号
    
    RePairData() {
        index_num = OUT_OF_RANGE;
        prev_index_num = OUT_OF_RANGE;
        next_index_num = OUT_OF_RANGE;
        prev_bigram_index_num = OUT_OF_RANGE;
        next_bigram_index_num = OUT_OF_RANGE;
    }

    RePairData(const RePairSymbol<T>& repair_symbol, const std::size_t& index_num, const std::size_t& prev_index_num, const std::size_t& next_index_num, const std::size_t& prev_bigram_index_num=OUT_OF_RANGE, const std::size_t& next_bigram_index_num=OUT_OF_RANGE)
    : repair_symbol(repair_symbol), index_num(index_num), prev_index_num(prev_index_num),  next_index_num(next_index_num), prev_bigram_index_num(prev_bigram_index_num), next_bigram_index_num(next_bigram_index_num) {}

    std::string to_string() const {
        std::string str = "(symbol: " + repair_symbol.to_string() + ", index_num: ";
        if (index_num == OUT_OF_RANGE)
            str += "-1, ";
        else
            str += std::to_string(index_num) + ", ";
        
        str += "prev_index_num: ";

        if (prev_index_num == OUT_OF_RANGE)
            str += "-1, ";
        else
            str += std::to_string(prev_index_num) + ", ";
        
        str += "next_index_num: ";

        if (next_index_num == OUT_OF_RANGE)
            str += "-1, ";
        else
            str += std::to_string(next_index_num) + ", ";
        
        str += "prev_bigram_index_num: ";

        if (prev_bigram_index_num == OUT_OF_RANGE)
            str += "-1, ";
        else
            str += std::to_string(prev_bigram_index_num) + ", ";

        str += "next_bigram_index_num: ";

        if (next_bigram_index_num == OUT_OF_RANGE)
            str += "-1)";
        else
            str += std::to_string(next_bigram_index_num) + ")";
        
        return str;
    }
};

// RePairDataのvector
template <typename T>
struct RePairDataList : std::vector<RePairData<T>> {
    RePairDataList() {}

    RePairDataList(const RePairData<T>& repair_data) : std::vector<RePairData<T>>({repair_data}) {}

    RePairDataList(const RePairDataList<T>& repair_data_list) : std::vector<RePairData<T>>(repair_data_list) {}

    RePairDataList(const std::initializer_list<RePairData<T>>& initializer_list) : std::vector<RePairData<T>>(initializer_list) {}

    std::string to_string() const {
        std::string str = "{";
        for (std::size_t i = 0; i < this->size();) {
            RePairData<T> repair_data = this->operator[](i);
            str += repair_data.to_string();
            i = repair_data.next_index_num;
            if (i != OUT_OF_RANGE)
                str += ", ";
        }
        str += "}";

        return str;
    }

    // index_numから始まるバイグラムの取得
    Bigram<T> get_bigram(const std::size_t& index_num) const {
        try {
            if (index_num >= this->size() - 1 && this->operator[](index_num).next_index_num == OUT_OF_RANGE)
                throw std::out_of_range("index_num is out of range.");
        } catch (const std::out_of_range& e) {
            std::cerr << e.what() << std::endl;
            std::exit(1);
        }

        return Bigram(this->operator[](index_num).repair_symbol, this->operator[](this->operator[](index_num).next_index_num).repair_symbol);
    }

    // index_numの位置のrepair_dataをないものとして扱うようにする
    void delete_repair_data(const std::size_t& index_num) {
        const std::size_t prev_index_num = this->operator[](index_num).prev_index_num; // RepairDataListにおける直前のデータのインデックス番号
        const std::size_t next_index_num = this->operator[](index_num).next_index_num; // RepairDataListにおける直後のデータのインデックス番号
        const std::size_t prev_bigram_index_num = this->operator[](index_num).prev_bigram_index_num; // 直前の同じバイグラムのインデックス番号
        const std::size_t next_bigram_index_num = this->operator[](index_num).next_bigram_index_num; // 直後の同じバイグラムのインデックス番号
        
        // index_numを更新
        if (prev_index_num != OUT_OF_RANGE)
            this->operator[](prev_index_num).next_index_num = next_index_num;
        if (next_index_num != OUT_OF_RANGE)
            this->operator[](next_index_num).prev_index_num = prev_index_num;

        // bigram_index_numを更新
        if (prev_bigram_index_num != OUT_OF_RANGE)
            this->operator[](prev_bigram_index_num).next_bigram_index_num = next_bigram_index_num;
        if (next_bigram_index_num != OUT_OF_RANGE)
            this->operator[](next_bigram_index_num).prev_bigram_index_num = prev_bigram_index_num;
    }

    // 非終端記号への置き換え
    void replace_with_nonterminal_symbol(const std::size_t& index_num, const NonTerminalSymbol& nonterminal_symbol) {
        delete_repair_data(this->operator[](index_num).next_index_num);
        this->operator[](index_num).repair_symbol = RePairSymbol<T>(nonterminal_symbol);
    }

    // 実際に意味のあるRePairDataいくつ保有しているか
    std::size_t actual_size() {
        std::size_t count = 0;
        std::size_t index_num = 0;
        
        while (true) {
            count++;
            index_num = this->operator[](index_num).next_index_num;
            if (index_num == OUT_OF_RANGE)
                break;
        }

        return count;
    }
};

// バイグラムのレコード
struct BigramRecord {
    std::size_t first_location; // バイグラムの初出の位置
    std::size_t appearance_frequency; // バイグラムの出現頻度

    BigramRecord();

    BigramRecord(const std::size_t& first_location, const std::size_t& appearance_frequency);

    std::string to_string() const;
};

// ハッシュテーブル（バイグラムに対する出現頻度と最初に出る場所）
template <typename T>
struct HashTable : std::unordered_map<Bigram<T>, std::list<BigramRecord>::iterator> {
    std::string to_string() const {
        std::string str = "[";
        for (auto itr = this->begin(); itr != this->end(); itr++)
            str += "(" + itr->first.to_string() + " " + itr->second->to_string() + "), ";

        if (this->begin() != this->end())
            str.erase(str.size() - 2, 2);
        
        str += "]";

        return str; 
    }
};

// 優先度付きキュー
struct PriorityQueue : std::vector<std::list<BigramRecord>> {
    PriorityQueue();

    PriorityQueue(const std::size_t& size, const BigramRecord& bigram_record);
    
    PriorityQueue(const std::size_t& size);
    
    std::string to_string () const;
};

// aaaなどの連続する箇所の変更を保持する構造体
struct ConsecutiveSymbolData {
    std::size_t consecutive_count; // いくつ連続しているか
    bool is_begin; // 連続する箇所のはじめであるか
    std::size_t other_end_index_num; // 連続する領域のもう片方の端のインデックス番号

    ConsecutiveSymbolData(const std::size_t& consecutive_count=0, const bool& is_begin=false, const std::size_t& other_end_index_num=OUT_OF_RANGE);

    std::string to_string() const;
};

// ConsecutiveSymbolDataListのvector
struct ConsecutiveSymbolDataList : std::vector<ConsecutiveSymbolData> {
    std::string to_string() const;

    void set_consecutive_data(const std::size_t& consecutive_count, const std::size_t& begin_index_num, const std::size_t& end_index_num);

    void update_consecutive_symbol(const std::size_t& index_num, const std::size_t& new_end_index_num);

    void delete_consecutive_symbol(const std::size_t& index_num);
};

#endif