#ifndef REPAIR_HPP
#define REPAIR_HPP

#include <string>
#include <vector>
#include "types.hpp"

template <typename T>
class RePair {
    private:
        RePairText<T> text;
        RePairText<T> compressed_text;
        RePairText<T> decompressed_text;
        Rules<T> rules;

        void decompress() {

        }

    public:
        void compress() {

        }

        bool is_equal_text_and_decompressed_text() {
            decompress();
            return text == decompressed_text;
        }
};

template <>
class RePair<char> {

};

#endif