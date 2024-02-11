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

std::string BigramRecord::to_string() const {
    return "first_location: " + std::to_string(first_location) + ", appearance_frequency: " + std::to_string(appearance_frequency);
}

std::string PriorityQueue::to_string() const {
    std::string str = "[";

    for (const std::list<BigramRecord>& bigram_record_list : *this) {
        str += "{";
        
        for (auto itr = bigram_record_list.begin(); itr != bigram_record_list.end(); itr++)
            str += "<" + itr->to_string() + ">, ";

        str.erase(str.size() - 2, 2);
    }

    str.erase(str.size() - 2, 2);
    str += "]";

    return str;
}

std::string ConsecutiveSymbolData::to_string() const {
    return "consecutive_count: " + std::to_string(consecutive_count) + ", is_begin: " + std::to_string(is_begin) + ", other_end_index_num: " + std::to_string(other_end_index_num);
}

std::string ConsecutiveSymbolDataList::to_string() const {
    std::string str = "{";
    
    for (std::size_t i = 0; i < this->size(); i++) {
        if (this->operator[](i).consecutive_count != 0)
            str += "[index_num: " + std::to_string(i) + ", " + this->operator[](i).to_string() + "], ";
    }

    str.erase(str.size() - 2, 2);
    str += "}";
    
    return str;
}

void ConsecutiveSymbolDataList::update_consecutive_symbol(const std::size_t& index_num, const std::size_t& new_end_index_num) {
    try {
        if (this->operator[](index_num).consecutive_count == 0)
            throw std::invalid_argument("index num is wrong");
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    const std::size_t new_consecutive_count = this->operator[](index_num).consecutive_count - 1;

    if (new_consecutive_count >= 2) {
        const std::size_t other_end_index_num = this->operator[](index_num).other_end_index_num;

        this->operator[](new_end_index_num).consecutive_count = new_consecutive_count;
        this->operator[](new_end_index_num).is_begin = this->operator[](new_end_index_num).is_begin;
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