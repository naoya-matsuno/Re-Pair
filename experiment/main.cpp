// #include "types.hpp"
#include <iostream>
#include <string>
#include <typeinfo>

int main() {
    std::string  a = "aaa";
    std::cout << typeid(a).name() << std::endl;
    std::cout << typeid("aaa").name() << std::endl;
}