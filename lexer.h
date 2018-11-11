#include <utility>

#pragma once
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


class Buffer {
private:
    std::string filename_;
    std::ifstream input_file;
    int c_;
    int line_, column_;
public:
    explicit Buffer(const std::string &filename) : filename_(filename), line_(1),column_(0) {
        input_file = std::ifstream(filename);
        c_ = input_file.get();
    }

    int next_char() {
        int t = c_;
        column_++;
        if (t == '\n'){
            line_++;
            column_ = 0;
        }
        c_ = input_file.get();
        return t;
    }

    int peak() {
        return c_;
    }

    int line(){
        return line_;
    }

    int column(){
        return column_;
    }

    void return_char(char c) {
        input_file.putback(c_);
        c_ = c;
    }
};

/* print variant type to stream */
template <typename T0, typename ... Ts>
std::ostream & operator<< (std::ostream & s,
                           std::variant<T0, Ts...> const & v)
{ std::visit([&](auto && arg){ s << arg;}, v); return s; }

class Token {
public:
    Token (): type_(token_types::UNKNOWN){}
    Token (token_types type): type_(type){}

    Token(int line, int column, token_types type, std::variant<int,float,std::string> value, std::string text):
    line_(line), column_(column), type_(type), value_(std::move(value)), text_(std::move(text)){}

    std::string print(){
        std::stringstream buffer;
        buffer << std::setw(5) << line_ << std::setw(8) << column_ << std::setw(14)
        << token_types_names[type_] << std::setw(15) << value_ << std::setw(15) << text_ << std::endl;
        return buffer.str();
    }

    int line_  = 0;
    int column_ = 0;
    token_types type_ = token_types::UNKNOWN;
    std::variant<int, float, std::string> value_ = 0;
    std::string text_ = "";
};

class LexerException : public std::exception {
protected:
    std::string m_msg;
public:

    LexerException(int line, int column) {
        m_msg = ("LexerError at line "+std::to_string(line)+" column "+std::to_string(column));
    }

    virtual const char* what() const throw()
    {
        return m_msg.c_str();
    }
};

class IncorrectOperatorException : public LexerException {
public:
    IncorrectOperatorException(int line, int column) : LexerException(line, column) {
        m_msg = ("LexerError: Incorrect operator at line "+std::to_string(line)+" column "+std::to_string(column));
    }
};

class IncorrectCharacterException : public LexerException {
public:
    IncorrectCharacterException(int line, int column) : LexerException(line, column) {
        m_msg = "LexerError: Incorrect character at line "+std::to_string(line)+" column "+std::to_string(column);
    }
};

class UnterminatedStringException : public LexerException {
public:
    UnterminatedStringException(int line, int column) : LexerException(line, column) {
        m_msg = ("LexerError: Unterminated string at line "+std::to_string(line)+" column "+std::to_string(column));
    }
};

class UnterminatedCommentException : public LexerException {
public:
    UnterminatedCommentException(int line, int column) : LexerException(line, column) {
        m_msg = ("LexerError: Unterminated comment at line "+std::to_string(line)+" column "+std::to_string(column));
    }
};


class Lexer {
public:
    Lexer(const std::string &filename) : buffer_(filename) {}

    Token get_next() {
        while (true){
            int c = buffer_.peak();
            if ((c == ' ') || c == '\n'){
                c = buffer_.next_char();
                continue;
            } else if (c == '{'){
                while (true){
                    c = buffer_.next_char();
                    if (c == std::char_traits<int>::eof()){
                        throw UnterminatedCommentException(buffer_.line(),buffer_.column());
                    }
                    if (c == '}'){
                        break;
                    }
                }
                continue;
            } else if (isalpha(c)){
                return read_identificator();
            } else if (isdigit(c) || (c == '%') || (c == '$')){
                return read_number();
            } else if (is_operator_symbol(c)){
                return read_relop();
            } else if (c == std::char_traits<int>::eof()){
                return {token_types ::ENDOFFILE};
            } else if ((c == '\'') || (c == '#')){
                return read_string();
            } else{
                c = buffer_.next_char();
                throw IncorrectCharacterException(buffer_.line(),buffer_.column());
            }
        }
    }

private:
    Buffer buffer_;

    std::set<int> operator_symbols{'>','<','=','!',':','+','-','/','*',';',',','.'};
    bool is_operator_symbol(int c){
        return static_cast<bool>(operator_symbols.count(c));
    };

    Token read_relop() {
        std::string s;
        int c = buffer_.peak();
        while (is_operator_symbol(c)){
            c = do_buffer_step(s, c);
        }
        if (operators.find(s) != operators.end()){
            return {buffer_.line(), buffer_.column(), token_types ::OPERATOR, token_types_names[operators[s]],
                    s};
        } else{
            //throw exception
            throw IncorrectOperatorException(buffer_.line(), buffer_.column());
        }
    }

    Token read_identificator() {
        std::string s;
        int c = buffer_.peak();
        while (isalpha(c) || isdigit(c)) {
            c = do_buffer_step(s, c);
        }
        if (keywords.find(s) == keywords.end()) {
            std::string s_lower(s);
            std::transform(s_lower.begin(), s_lower.end(), s_lower.begin(), ::tolower);
            return {buffer_.line(), buffer_.column(), token_types::IDENTIFICATOR, s_lower, s};
        } else {
            return {buffer_.line(), buffer_.column(), token_types::KEYWORD, token_types_names[keywords[s]], s};
        }
    }

    Token read_number(){
        std::string s;
        int c = buffer_.peak();
        int state = 0;
        enum {START, INT, FLOAT, BIN, HEX, SCALEFACTOR};
        while(true){
            switch (state){
                case START:{
                    if (isdigit(c)){
                        state = INT;
                    } else if (c == '$'){
                        state = HEX;
                        c = do_buffer_step(s, c);
                    } else if (c == '%'){
                        c = do_buffer_step(s, c);
                        state = BIN;
                    }
                    break;
                }

                case INT:{
                    while (isdigit(c)){
                        c = do_buffer_step(s, c);
                    }
                    if (c == '.' || c == 'E' || c == 'e'){
                        state = FLOAT;
                    } else{
                        return {buffer_.line(),buffer_.column(), token_types ::INTEGER, std::stoi(s), s};
                    }
                    break;
                }

                case FLOAT:{
                    c = buffer_.peak();
                    if (c == '.'){
                        c = do_buffer_step(s, c);
                    }
                    while(isdigit(c)){
                        c = do_buffer_step(s, c);
                    }
                    if (c == 'E' || c == 'e'){
                        state = SCALEFACTOR;
                        c = do_buffer_step(s, c);
                        while (isdigit(c)){
                            c = do_buffer_step(s, c);
                        }
                        return {buffer_.line(),buffer_.column(),token_types ::FLOAT, std::stof(s), s};
                    } else {
                        return {buffer_.line(),buffer_.column(),token_types ::FLOAT, std::stof(s), s};
                    }
                }


                case HEX:{
                    while (isxdigit(c)){
                        c = do_buffer_step(s ,c);
                    }
                    std::string hex_string = "0x"+s;
                    hex_string.erase(2,1);
                    return {buffer_.line(), buffer_.column(), token_types ::INTEGER, int(std::stof(hex_string)), s};
                }

                case BIN:{
                    while ((c == '0' || c == '1')){
                        c = do_buffer_step(s ,c);
                    }
                    std::string bin_string(s);
                    bin_string.erase(0,1);
                    return {buffer_.line(), buffer_.column(), token_types ::INTEGER, std::stoi(bin_string, nullptr,  2), s};
                }
            }
        }
    }

    int do_buffer_step(std::string &s, int c) {
        c = buffer_.next_char();
        s.push_back(char(c));
        c = buffer_.peak();
        return c;
    }

    Token read_string(){
        std::string s;
        int c = buffer_.peak();
        if (c == '\''){
            c = buffer_.next_char();
            c = buffer_.peak();
        } else{
            //throw exception
        }
        while(true){
            if (c == '\''){
                c = buffer_.next_char();
                c = buffer_.peak();
                if (c == '\''){
                    c = do_buffer_step(s, c);
                } else{
                    return {buffer_.line(), buffer_.column(), token_types::LITERAL, s, s};
                }
            } else if (c == '#'){
                c = buffer_.next_char();
                Token t = read_number();
                s.push_back(static_cast<char>(stoi(t.text_)));
                c = buffer_.peak();
                if ((c != '\'') && (c != '#')){
                    return {buffer_.line(), buffer_.column(), token_types::LITERAL, s, s};
                } else if (c == '\''){
                    c = buffer_.next_char();
                    c = buffer_.peak();
                }
            } else if (c == std::char_traits<int>::eof()) {
                throw UnterminatedStringException(buffer_.line(), buffer_.column());
            } else{
                    c = do_buffer_step(s, c);
            }
        }
    }

};

