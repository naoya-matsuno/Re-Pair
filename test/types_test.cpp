#include <iostream>
#include <unordered_map>
#include "types.hpp"

void terminal_symbol_test();
void nonterminal_symbol_test();
void repair_symbol_test();
void bigram_test();

int main() {
    terminal_symbol_test();
    nonterminal_symbol_test();
    repair_symbol_test();
    bigram_test();
}

// TerminalSymbolのテスト
void terminal_symbol_test() {
    TerminalSymbol terminal_symbol_A('a');
    TerminalSymbol terminal_symbol_B('b');
    TerminalSymbol terminal_symbol_C('a');

    if (terminal_symbol_A == terminal_symbol_B || terminal_symbol_A != terminal_symbol_C) {
        std::cerr << "TerminalSymbol operator== not working." << std::endl;
        exit(1);
    }

    if (terminal_symbol_B < terminal_symbol_A) {
        std::cerr << "TerminalSymbol operator< not working." << std::endl;
        exit(1);
    }

    if (terminal_symbol_A > terminal_symbol_B) {
        std::cerr << "TerminalSymbol operator> not working." << std::endl;
        exit(1);
    }
}

// NonTerminalSymbolのテスト
void nonterminal_symbol_test() {
    NonTerminalSymbol nonterminal_symbol_A(0);
    NonTerminalSymbol nonterminal_symbol_B(1);
    NonTerminalSymbol nonterminal_symbol_C(0);

    if (nonterminal_symbol_A == nonterminal_symbol_B || nonterminal_symbol_A != nonterminal_symbol_C) {
        std::cerr << "NonTerminalSymbol operator== not working." << std::endl;
        exit(1);
    }

    if (nonterminal_symbol_B < nonterminal_symbol_A) {
        std::cerr << "NonTerminalSymbol operator< not working." << std::endl;
        exit(1);
    }

    if (nonterminal_symbol_A > nonterminal_symbol_B) {
        std::cerr << "NonTerminalSymbol operator> not working." << std::endl;
        exit(1);
    }
}

// RePairSymbolのテスト
void repair_symbol_test() {
    RePairSymbol repair_symbol_A(TerminalSymbol('a'));
    RePairSymbol repair_symbol_B(TerminalSymbol('b'));
    RePairSymbol repair_symbol_C(TerminalSymbol('a'));
    RePairSymbol<char> repair_symbol_D(NonTerminalSymbol(0));
    RePairSymbol<char> repair_symbol_E(NonTerminalSymbol(1));
    RePairSymbol<char> repair_symbol_F(NonTerminalSymbol(0));

    if (repair_symbol_A == repair_symbol_B || repair_symbol_A != repair_symbol_C || repair_symbol_D == repair_symbol_E || repair_symbol_D != repair_symbol_F || repair_symbol_A == repair_symbol_D) {
        std::cerr << "RePairSymbol operator== not working." << std::endl;
        exit(1);
    }

    if (repair_symbol_B < repair_symbol_A || repair_symbol_F < repair_symbol_D || repair_symbol_A < repair_symbol_D) {
        std::cerr << "RePairSymbol operator< not working." << std::endl;
        exit(1);
    }
}

// Bigramのテスト
void bigram_test() {
    Bigram bigram_A(RePairSymbol(TerminalSymbol('a')), RePairSymbol(TerminalSymbol('b')));
    Bigram bigram_B(RePairSymbol(TerminalSymbol('c')), RePairSymbol(TerminalSymbol('d')));
    Bigram bigram_C(RePairSymbol(TerminalSymbol('a')), RePairSymbol(TerminalSymbol('b')));
    std::unordered_map<Bigram<char>, std::size_t> unordered_map_A;

    unordered_map_A[bigram_A] = 1;
    unordered_map_A[bigram_B] = 1;

    if (unordered_map_A.at(bigram_A) != 1 || unordered_map_A.at(bigram_B) != 1) {
        std::cerr << "Bigram is no working as key of unordered_map" << std::endl;
        exit(1);
    }

    if (!unordered_map_A.contains(bigram_C) || unordered_map_A.at(bigram_C) != 1) {
        std::cerr << "Bigram's hash caluculation is wrong." << std::endl;
        exit(1);
    }
}