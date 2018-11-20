#pragma once

#include <utility>
#include <utility>
#include <iostream>
#include <string>
#include <ctype.h>
#include <iomanip>
#include <map>
#include <fstream>
#include <set>
#include <algorithm>
#include "token_types.h"
#include <exception>
#include <variant>
#include "exceptions.h"

class Buffer {
public:
    explicit Buffer(const std::string &filename);

    int next_char();

    int peek() {
        return c_;
    }

    int line() {
        return line_;
    }

    int column() {
        return column_;
    }

    void return_char(char c);

private:
    std::string filename_;
    std::ifstream input_file;
    int c_;
    int line_, column_;
};

class Token {
public:
    Token(Buffer& buffer, token_types type) : line_(buffer.line()), column_(buffer.column()), type_(type) {}

    Token(Buffer& buffer, token_types type, std::variant<int, float, std::string> value, std::string text) :
        line_(buffer.line()), column_(buffer.column()), type_(type), value_(std::move(value)), text_(std::move(text)) {}

    std::string print();

    std::pair<int,int> position() { return std::make_pair(line_,column_);}
    token_types type(){ return type_;}
    std::variant<int, float, std::string> value(){ return value_;}
    std::string text() { return text_;}
private:
    int line_ = 0;
    int column_ = 0;
    token_types type_ = token_types::UNKNOWN;
    std::variant<int, float, std::string> value_ = 0;
    std::string text_ = "";
};


class Lexer {
public:
    explicit Lexer(const std::string &filename) : buffer_(filename) {}

    Token get_next();

private:
    Buffer buffer_;

    std::set<int> operator_symbols{'>', '<', '=', '!', ':', '+', '-', '/', '*', ';', ',', '.', '[', ']', '(', ')', '@',
                                   '^', '<', '>'};

    bool is_operator_symbol(int c) {
        return static_cast<bool>(operator_symbols.count(c));
    };

    Token read_relop();

    Token read_identifier();

    Token read_number();

    int do_buffer_step(std::string &s, int c) {
        c = buffer_.next_char();
        s.push_back(char(c));
        c = buffer_.peek();
        return c;
    }

    Token read_string();

};

