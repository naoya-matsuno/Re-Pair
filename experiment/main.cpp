#include <iostream>
#include <vector>
#include <string>

template<typename T = char>
class MyVector : public std::vector<T> {
public:
    // コンストラクタの引数が string の場合
    template<typename U = T>
    MyVector(const std::string& str,
             typename std::enable_if<std::is_same<U, char>::value>::type* = nullptr)
    : std::vector<T>(str.begin(), str.end()) {
        std::cout << "Constructed from string" << std::endl;
    }

    // その他の場合
    template<typename U = T>
    MyVector(const U& val,
             typename std::enable_if<!std::is_same<U, std::string>::value && std::is_convertible<U, T>::value>::type* = nullptr)
    : std::vector<T>(1, val) {
        std::cout << "Constructed from non-string" << std::endl;
    }
};

int main() {
    // 文字列から MyVector を構築
    MyVector charVector("Hello");

    // int から MyVector を構築
    MyVector intVector(5);

    return 0;
}
