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

#include "types.hpp"

template <>
std::string TerminalSymbol<char>::to_string() const {
    if (value == '\n')
        return "\\n";
    else
        return std::string(1, value);
}

NonTerminalSymbol::NonTerminalSymbol() {}

NonTerminalSymbol::NonTerminalSymbol(const std::size_t& value) : value(value) {}

NonTerminalSymbol::NonTerminalSymbol(const NonTerminalSymbol& nonterminal_symbol) : value(nonterminal_symbol.value) {}

std::string NonTerminalSymbol::to_string() const {
    return "(" + std::to_string(value) + ")";
}

bool NonTerminalSymbol::operator==(const NonTerminalSymbol& rhs) const {
    return this->value == rhs.value;
}

bool NonTerminalSymbol::operator<(const NonTerminalSymbol& rhs) const {
    return this->value < rhs.value;
}

bool NonTerminalSymbol::operator>(const NonTerminalSymbol& rhs) const {
    return this->value > rhs.value;
}


BigramRecord::BigramRecord() {}

BigramRecord::BigramRecord(const std::size_t& first_location, const std::size_t& appearance_frequency) : first_location(first_location), appearance_frequency(appearance_frequency) {}

std::string BigramRecord::to_string() const {
    return "first_location: " + std::to_string(first_location) + ", appearance_frequency: " + std::to_string(appearance_frequency);
}


PriorityQueue::PriorityQueue() {}

PriorityQueue::PriorityQueue(const std::size_t& size) : std::vector<std::list<BigramRecord>>(size) {}

PriorityQueue::PriorityQueue(const std::size_t& size, const BigramRecord& bigram_record) : std::vector<std::list<BigramRecord>>(size, std::list<BigramRecord>{bigram_record}) {}

std::string PriorityQueue::to_string() const {
    std::string str = "[";

    std::size_t index_num = 0;
    for (const std::list<BigramRecord>& bigram_record_list : *this) {
        str += std::to_string(index_num) + ": {";
        
        for (auto itr = bigram_record_list.begin(); itr != bigram_record_list.end(); itr++) {
            str += "<" + itr->to_string() + ">,\n";
        }
        
        if (this->operator[](index_num).size() > 0)
            str.erase(str.size() - 2, 2);
        str += "},\n";

        index_num++;
    }

    str.erase(str.size() - 2, 2);
    str += "]";

    return str;
}


ConsecutiveSymbolData::ConsecutiveSymbolData(const std::size_t& consecutive_count, const bool& is_begin, const std::size_t& other_end_index_num)
    : consecutive_count(consecutive_count), is_begin(is_begin), other_end_index_num(other_end_index_num) {}

std::string ConsecutiveSymbolData::to_string() const {
    return "consecutive_count: " + std::to_string(consecutive_count) + ", is_begin: " + std::to_string(is_begin) + ", other_end_index_num: " + std::to_string(other_end_index_num);
}


ConsecutiveSymbolDataList::ConsecutiveSymbolDataList() {}

ConsecutiveSymbolDataList::ConsecutiveSymbolDataList(const std::size_t& size) : std::vector<ConsecutiveSymbolData>(size) {}

ConsecutiveSymbolDataList::ConsecutiveSymbolDataList(const std::size_t& size, const ConsecutiveSymbolData& consecutive_symbol_data) : std::vector<ConsecutiveSymbolData>(size, consecutive_symbol_data) {}

std::string ConsecutiveSymbolDataList::to_string() const {
    std::string str = "{";
    
    for (std::size_t i = 0; i < this->size(); i++) {
        if (this->operator[](i).consecutive_count != 0)
            str += "[index_num: " + std::to_string(i) + ", " + this->operator[](i).to_string() + "],\n";
    }

    if (this->size() > 0)
        str.erase(str.size() - 2, 2);
    str += "}";
    
    return str;
}

void ConsecutiveSymbolDataList::set_consecutive_data(const std::size_t& consecutive_count, const std::size_t& begin_index_num, const std::size_t& end_index_num) {
    this->operator[](begin_index_num) = ConsecutiveSymbolData(consecutive_count, true, end_index_num);
    this->operator[](end_index_num) = ConsecutiveSymbolData(consecutive_count, false, begin_index_num);
}

void ConsecutiveSymbolDataList::update_consecutive_symbol(const std::size_t& index_num, const std::size_t& new_end_index_num) {
    try {
        if (this->operator[](index_num).consecutive_count == 0)
            throw std::invalid_argument("index_num is wrong. " + this->operator[](index_num).to_string());
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    const std::size_t new_consecutive_count = this->operator[](index_num).consecutive_count - 1;

    if (new_consecutive_count >= 2) {
        const std::size_t other_end_index_num = this->operator[](index_num).other_end_index_num;

        this->operator[](new_end_index_num).consecutive_count = new_consecutive_count;
        this->operator[](new_end_index_num).is_begin = this->operator[](index_num).is_begin;
        this->operator[](new_end_index_num).other_end_index_num = other_end_index_num;
        
        this->operator[](other_end_index_num).consecutive_count = new_consecutive_count;
        this->operator[](other_end_index_num).other_end_index_num = new_end_index_num;

        this->operator[](index_num).consecutive_count = 0;
        this->operator[](index_num).is_begin = false;
        this->operator[](index_num).other_end_index_num = OUT_OF_RANGE;
    } else
        delete_consecutive_symbol(index_num);
}

void ConsecutiveSymbolDataList::delete_consecutive_symbol(const std::size_t& index_num) {
    const std::size_t other_end_index_num = this->operator[](index_num).other_end_index_num;

    this->operator[](index_num).consecutive_count = 0;
    this->operator[](index_num).is_begin = false;
    this->operator[](index_num).other_end_index_num = OUT_OF_RANGE;

    this->operator[](other_end_index_num).consecutive_count = 0;
    this->operator[](other_end_index_num).is_begin = false;
    this->operator[](other_end_index_num).other_end_index_num = OUT_OF_RANGE;
}