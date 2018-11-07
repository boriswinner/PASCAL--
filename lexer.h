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


class Buffer {
private:
    std::string filename_;
    std::ifstream input_file;
    int c_;
public:
    explicit Buffer(const std::string &filename) : filename_(filename) {
        input_file = std::ifstream(filename);
        c_ = input_file.get();
    }

    int next_char() {
        int t = c_;
        c_ = input_file.get();
        return t;
    }

    int peak() {
        return c_;
    }

//    void return_char(char c) {
//        c_ = c;
//    }
};

class Token {
public:
    Token(int line, int column, token_types type, token_subtypes subtype) :
            line_(line), column_(column), type_(type), subtype_(subtype) {
        text_ = "";
    }

    Token(): type_(ENDOFFILE), subtype_(token_subtypes::UNKNOWN) {}

    Token(int line, int column, token_types type, std::string text) : line_(line), column_(column), type_(type),
                                                                 text_(std::move(text)) {
        subtype_ = token_subtypes::UNKNOWN;
    }

    Token(int line, int column, token_types type, token_subtypes subtype, std::string text) : line_(line), column_(column), type_(type), subtype_(subtype),
                                                                                         text_(std::move(text)) {}

    std::string print() {
        std::stringstream buffer;
        buffer << std::setw(10) << "line" << std::setw(10) << "column" << std::setw(13) << "type" << std::setw(12) << "subtype" << std::setw(15)
             << "text" << std::endl;
        buffer << std::setw(10) << line_ << std::setw(10) << column_ << std::setw(13) << token_types_names[type_] << std::setw(12)
             << token_subtypes_names[subtype_] << std::setw(15) << text_ << std::endl << std::endl;
        return buffer.str();
    }

    int line_;
    int column_;
    token_types type_;
    token_subtypes subtype_;
    std::string text_;
};

class Lexer {
public:
    Lexer(const std::string &filename) : buffer_(filename), line_(0), column_(0) {}

    Token get_next() {
        while (true){
            int c = buffer_.peak();
            column_++;
            if (c == '\n'){
                line_++;
                column_ = 0;
                c = buffer_.next_char();
                continue;
            } else if (c == ' '){
                c = buffer_.next_char();
                continue;
            } else if (isalpha(c)){
                return read_identificator();
            } else if (isdigit(c) || (c == '%') || (c == '$')){
                return read_number();
            } else if (is_operator_symbol(c)){
                return read_relop();
            } else if (is_separator_symbol(c)){
                return read_separator();
            } else if (c == std::char_traits<int>::eof()){
                return {};
            } else if ((c == '\'') || (c == '#')){
                return read_string();
            }
        }
    }

private:
    int line_, column_;
    Buffer buffer_;

    std::set<int> operator_symbols{'>','<','=','!',':','+','-','/','*'};
    bool is_operator_symbol(int c){
        return static_cast<bool>(operator_symbols.count(c));
    };

    std::set<int> separator_symbols{'(',')','[',']',':',';'};
    bool is_separator_symbol(int c){
        return static_cast<bool>(separator_symbols.count(c));
    };

    Token read_relop() {
        std::string s;
        int c = buffer_.peak();
        while (is_operator_symbol(c)){
            c = do_buffer_step(s, c);
        }
        if (operators.find(s) != operators.end()){
            return {line_, column_, token_types::OPERATOR, s};
        } else{
            //throw exception
        }
    }

    Token read_separator(){
        std::string s;
        int c = buffer_.peak();
        while (is_separator_symbol(c)){
            c = do_buffer_step(s, c);
        }
        if (operators.find(s) != operators.end()){
            return {line_, column_, token_types::OPERATOR, s};
        } else{
            //throw exception
        }
    }

    Token read_identificator() {
        std::string s;
        int c = buffer_.peak();
        while (isalpha(c) || isdigit(c)) {
            c = do_buffer_step(s, c);
        }
        if (keywords.find(s) == keywords.end()) {
            return {line_, column_, token_types::IDENTIFICATOR, s};
        } else {
            return {line_, column_, token_types::KEYWORD, keywords[s], s};
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
                        return {line_,column_, token_types ::NUMBER, token_subtypes::INTEGER, s};
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
                        return {line_,column_,token_types ::NUMBER,token_subtypes::FLOAT, s};
                    } else {
                        return {line_,column_, token_types ::NUMBER, token_subtypes::FLOAT, s};
                    }
                }


                case HEX:{
                    while (isxdigit(c)){
                        c = do_buffer_step(s ,c);
                    }
                    return {line_, column_, token_types ::NUMBER, token_subtypes::INTEGER, s};
                }

                case BIN:{
                    while ((c == '0' || c == '1')){
                        c = do_buffer_step(s ,c);
                    }
                    return {line_, column_, token_types ::NUMBER, token_subtypes::INTEGER, s};
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
                    return {line_, column_, token_types::LITERAL, s};
                }
            } else if (c == '#'){
                c = buffer_.next_char();
                Token t = read_number();
                s.push_back(static_cast<char>(stoi(t.text_)));
                c = buffer_.peak();
                if ((c != '\'') && (c != '#')){
                    return {line_, column_, token_types::LITERAL, s};
                } else if (c == '\''){
                    c = buffer_.next_char();
                    c = buffer_.peak();
                }
            } else{
                c = do_buffer_step(s, c);
            }
        }
    }

};

